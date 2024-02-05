#include "LSystemCmd.h"
#include "LSystem.h"

#include <maya/MGlobal.h>
#include <list>
#pragma warning(disable : 2059)

struct Grammar {
	std::string start;
	std::_In_place_key_extract_map<char, std::string> rules;
};

LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

const char* stepSizeFlag = "-s", * stepSizeLongFlag = "-step"; // double
const char* degreeFlag = "-d", * degreeLongFlag = "-degree"; //double
const char* grammarFlag = "-g", * grammarLongFlag = "-grammar"; //string
const char* iterationFlag = "-i", * iterationLongFlag = "-iteration"; // int

MSyntax LSystemCmd::newSyntax() {
	MSyntax syntax;
	syntax.addFlag(stepSizeFlag, stepSizeLongFlag, MSyntax::kDouble);
	syntax.addFlag(degreeFlag, degreeLongFlag, MSyntax::kDouble);
	syntax.addFlag(grammarFlag, grammarLongFlag, MSyntax::kString);
	syntax.addFlag(iterationFlag, iterationLongFlag, MSyntax::kDouble);
	return syntax;
}

MString drawShape(vec3 start, vec3 end) {
	double nurbCircleX = end[0] - start[0];
	double nurbCircleY = end[1] - start[1];
	double nurbCircleZ = end[2] - start[2];
	
	MString createShapeCmd = "curve -d 1 -p ";
	createShapeCmd += " ";
	createShapeCmd += start[0];
	createShapeCmd += " ";
	createShapeCmd += start[1];
	createShapeCmd += " ";
	createShapeCmd += start[2];
	createShapeCmd += " -p ";
	createShapeCmd += end[0];
	createShapeCmd += " ";
	createShapeCmd += end[1];
	createShapeCmd += " ";
	createShapeCmd += end[2];
	createShapeCmd += " -k 0 -k 1 -name curve1; circle -c 0 0 0 -nr ";
	createShapeCmd += nurbCircleX;
	createShapeCmd += " ";
	createShapeCmd += nurbCircleY;
	createShapeCmd += " ";
	createShapeCmd += nurbCircleZ;
	createShapeCmd += " -sw 360 -r 1 -d 3 -ut 0 -tol 0.01 -s 8 -ch 1 -name nurbsCircle1; select -r nurbsCircle1 curve1 ;extrude -ch true -rn false -po 1 -et 2 -ucp 1 -fpt 1 -upn 1 -rotation 0  -scale 1 -rsp 1 \"nurbsCircle1\" \"curve1\" ;";

	return createShapeCmd;
}

MStatus LSystemCmd::doIt( const MArgList& args )
{
	// message in Maya output window
    cout<<"Implement Me!"<<endl;
	std::cout.flush();

	// message in scriptor editor
	MGlobal::displayInfo("Implement Me!");

	LSystem ls;

	// get arguments from cmd line
	MString grammar;
	double step, degree, iter;
	MArgDatabase argData(newSyntax(), args);
	if (argData.isFlagSet(stepSizeFlag)) {
		argData.getFlagArgument(stepSizeFlag, 0, step);
		ls.setDefaultStep(step);
	}
	if (argData.isFlagSet(degreeFlag)) {
		argData.getFlagArgument(degreeFlag, 0, degree);
		ls.setDefaultAngle(degree);
	}
	if (argData.isFlagSet(grammarFlag)) {
		argData.getFlagArgument(grammarFlag, 0, grammar);
		grammar.substitute("__NEWLINE__", "\n");
		ls.loadProgramFromString(grammar.asChar());
	}
	if (argData.isFlagSet(iterationFlag)) {
		argData.getFlagArgument(iterationFlag, 0, iter);
	}



	MString cmd = "confirmDialog -title \"test L-system\" -message \"\" ";
	MString disInfo = "step size: ";
	disInfo += step;
	disInfo += " degree: ";
	disInfo += degree;
	disInfo += " grammar: ";
	disInfo += grammar;
	disInfo += " iteration: ";
	disInfo += iter;
	MGlobal::displayInfo(disInfo);

	//MString test_drawCmd = drawShape(0, 0, 0, 1, 2, 3);
	//MGlobal::executeCommand(test_drawCmd);
	//MGlobal::displayInfo(test_drawCmd);

	for (int i = 0; i < iter; i++) {
		std::vector<LSystem::Branch> branches;
		std::vector<LSystem::Geometry> geos;
		ls.process(i, branches, geos);
		for (int j = 0; j < branches.size(); j++) {
			MString drawBranchCmd = drawShape(branches[j].first, branches[j].second);
			MGlobal::executeCommand(drawBranchCmd);
		}
		
	}


    return MStatus::kSuccess;
}

