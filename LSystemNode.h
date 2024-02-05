#pragma once
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MGlobal.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>
#include <fstream>
#include <sstream>

#include "cylinder.h"
#include "LSystem.h"
const MTypeId LSystemId(0x00000222);

class LSystemNode :
    public MPxNode
{
public:
	LSystemNode() {};
	virtual 		~LSystemNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void* creator();
	static  MStatus initialize();

	static MObject angle;
	static MObject stepSize;
	static MObject grammar;
	static MObject time;
	static MObject outputMesh;
	static MTypeId id;

	static LSystem ls;

};


