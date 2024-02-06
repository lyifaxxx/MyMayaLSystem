#include "LSystemNode.h"
#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

MObject LSystemNode::angle;
MObject LSystemNode::stepSize;
MObject LSystemNode::grammar;
MObject LSystemNode::time;
MObject LSystemNode::outputMesh;
MTypeId LSystemNode::id(LSystemId);
LSystem LSystemNode::ls;

void* LSystemNode::creator() {
	return new LSystemNode;
}

MStatus LSystemNode::initialize() {
    MFnNumericAttribute nAttr;
    MFnTypedAttribute tAttr;
    MFnUnitAttribute uAttr;
    MStatus status;
    MObject defaultObj;

    // Default angle attribute
    LSystemNode::angle = nAttr.create("angle", "a", MFnNumericData::kDouble, 45.0, &status);
    nAttr.setKeyable(true);
    addAttribute(angle);

    // Default step size attribute
    LSystemNode::stepSize = nAttr.create("stepSize", "ss", MFnNumericData::kDouble, 1.0, &status);
    nAttr.setKeyable(true);
    addAttribute(stepSize);

    // Grammar file attribute
    LSystemNode::grammar = tAttr.create("grammar", "gm", MFnData::kString, defaultObj, &status);
    tAttr.setUsedAsFilename(true);
    addAttribute(grammar);

    // Input time attribute
    LSystemNode::time = uAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0, &status);
    addAttribute(time);
    McheckErr(status, "ERROR creating LSystemNode time attribute\n");

    // Output geometry attribute
    LSystemNode::outputMesh = tAttr.create("outputMesh", "om", MFnData::kMesh, defaultObj, &status);
    tAttr.setWritable(false);
    tAttr.setStorable(false);
    addAttribute(outputMesh);
    McheckErr(status, "ERROR creating LSystemNode outputMesh attribute\n");

    // Attribute affects relationships
    attributeAffects(angle, outputMesh);
    attributeAffects(stepSize, outputMesh);
    attributeAffects(grammar, outputMesh);
    attributeAffects(time, outputMesh);

    return MS::kSuccess;


}

MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data) {
    if (plug != outputMesh) {
        return MS::kUnknownParameter;
    }

    MStatus returnStatus;
    // Fetch input attribute values
    MDataHandle angleHandle = data.inputValue(angle);
    double angle = angleHandle.asDouble();

    MDataHandle stepSizeHandle = data.inputValue(stepSize);
    double stepSize = stepSizeHandle.asDouble();

    MDataHandle grammarFileHandle = data.inputValue(grammar);
    MString grammarPath = grammarFileHandle.asString();
    std::ifstream file(grammarPath.asChar());
    if (!file) {
        //MGlobal::displayError("Failed to open grammar file: " + grammarPath);
        return MS::kFailure;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    MString grammar= buffer.str().c_str();

    MDataHandle inputTimeHandle = data.inputValue(time, &returnStatus);
    MTime inputTime = inputTimeHandle.asTime();
    McheckErr(returnStatus, "Error getting time data handle\n");

    MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
    McheckErr(returnStatus, "ERROR getting polygon data handle\n");

    MFnMeshData dataCreator;
    MObject newOutputData = dataCreator.create(&returnStatus);
    McheckErr(returnStatus, "ERROR creating outputData\n");

    // Parsing the grammar and use cylinder to create a mesh
    // Create a L-system
    
    ls.setDefaultAngle(angle);
    ls.setDefaultStep(stepSize);
    ls.loadProgramFromString(grammar.asChar());// Parsing the grammar
    MString lsInfo = "angle:";
    lsInfo += angle;
    lsInfo += " stepSize: ";
    lsInfo += stepSize;
    lsInfo += "grammar: ";
    lsInfo += grammar;
    //MGlobal::displayInfo(lsInfo);

    std::vector<LSystem::Branch> branches;
    std::vector<LSystem::Geometry> geos;
    MString timeCount;
    timeCount.set(inputTime.value());
    MGlobal::displayInfo(timeCount);
    ls.process(inputTime.value(), branches, geos);
    //ls.process(2, branches, geos);
    MPointArray points;
    MIntArray faceCounts;
    MIntArray faceConnects;

    MString branSize;
    branSize.set(branches.size());
    //MGlobal::displayInfo(branSize);
    for (int i = 0; i < branches.size(); i++) {
        // Use the branch to create cylinders and attach them to a mesh
        
        MPoint start = MPoint(branches[i].first[0], branches[i].first[1], branches[i].first[2]);
        MPoint end = MPoint(branches[i].second[0], branches[i].second[1], branches[i].second[2]);
        CylinderMesh newMesh(start, end);
        // get mesh data from the output mesh
        //mesh.getMesh(points, faceCounts, faceConnects);
        // append the new mesh data to the output mesh data
        newMesh.appendToMesh(points, faceCounts, faceConnects);

    }

    // write the mesh data to MObject
    MFnMesh meshFS;
    MObject outputMesh = meshFS.create(points.length(), faceCounts.length(),
        points, faceCounts, faceConnects,
        newOutputData, &returnStatus);
    McheckErr(returnStatus, "ERROR creating output attribute\n");
    outputHandle.set(newOutputData);
    data.setClean(plug);

    return returnStatus;
}
