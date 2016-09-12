#pragma once
#include "struct_basic_types.h"

//////////////////////////
//struct_node_splitter.h//
//////////////////////////
const double cT = 1.;
const double cI = 2.;

const UI1 cNumBins = 33;
const UI1 cMaxElementsInNode = 32;

const double cNumBinsDouble = static_cast<double>(cNumBins);

////////////////////
//struct_tree_kd.h//
////////////////////
const UI1 cMaxDepthTree = 32;
const UI1 cNumElementsForParalell = 1024;

const UI1 cStringBufferSize = 512;
const UI1 cHyperVectorBufferSize = 1024;

//////////////////
//math constants//
//////////////////
const double cEps = 1e-8;
const double cEps2 = 1e-14;

///////////////////////////////
//test find nearest constants//
///////////////////////////////
const UI1 cNumRandomPoints = 10000;
const double cMinBoundary = -1000.;
const double cMaxBoundary = 1000.;
