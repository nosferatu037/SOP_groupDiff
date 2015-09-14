#include "sop_groupDiff.h"


//#include "SOP_groupDiff.h"

#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <GEO/GEO_AttributeHandle.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Matrix3.h>
#include <UT/UT_Matrix4.h>
#include <SYS/SYS_Math.h>
#include <typeinfo>
#include <iostream>
#include <vector>
#include <stddef.h>

using namespace HDK_Sample;
using std::vector;


void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "hdk_groupDiff",
        "Group Difference",
        SOP_groupDiff::myCtor,              //how to build a sop
        SOP_groupDiff::myTemplateList,      //mu ui parms
        1,                                  //min inputs
        2,                                  //max inputs
        0));                                //local vars
}

//set our attr lookup name
//static PRM_Name     useAttrib   ("useAttr", "Use Attributes");
static PRM_Name     attrLookup  ("attrLook", "Att to lookup");
static PRM_Name     putInGrp    ("createGrp", "Create new Group");
static PRM_Name     newGroup    ("newGrp", "Add To Group");
static PRM_Name     deletePts   ("delPts", "Delete Points");
//static PRM_Name     comparePos   ("getPos", "Compare P");
static PRM_Name     debugMe       ("debug", "Debug console msgs");

//static PRM_Conditional disAttr("{ useAttr == 1 }");
static PRM_Conditional disP("{ getPos == 1 }");
static PRM_Conditional disGrps("{ delPts == 1 }");
static PRM_Conditional disDel("{ createGrp == 1 }");


PRM_Template
SOP_groupDiff::myTemplateList[] = {
    PRM_Template(PRM_STRING, 1, &PRMgroupName,  0, &SOP_Node::pointGroupMenu, 0, 0, SOP_Node::getGroupSelectButton(GA_GROUP_POINT)),
//    PRM_Template(PRM_TOGGLE, 1, &useAttrib,     PRMoneDefaults, 0, 0, 0, 0, 0, 0, &disP),
    PRM_Template(PRM_STRING, 1, &attrLookup,    0, &SOP_Node::pointAttribMenu, 0, 0, 0, 0, 0, &disP),
    PRM_Template(PRM_TOGGLE, 1, &putInGrp,      PRMoneDefaults, 0, 0, 0, 0, 1, 0, &disGrps),
    PRM_Template(PRM_STRING, 1, &newGroup,      0, 0, 0, 0, 0, 1, 0, &disGrps),
    PRM_Template(PRM_TOGGLE, 1, &deletePts,     PRMzeroDefaults, 0, 0, 0, 0, 1, 0, &disDel),
//    PRM_Template(PRM_TOGGLE, 1, &comparePos,     PRMzeroDefaults, 0, 0, 0, 0, 1, 0, &disAttr),
    PRM_Template(PRM_TOGGLE, 1, &debugMe,       PRMzeroDefaults),
    PRM_Template(),
};


OP_Node *
SOP_groupDiff::myCtor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_groupDiff(net, name, op);
}

SOP_groupDiff::SOP_groupDiff(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op), myGroup(NULL)
{
    // This indicates that this SOP manually manages its data IDs,
    // so that Houdini can identify what attributes may have changed,
    // e.g. to reduce work for the viewport, or other SOPs that
    // check whether data IDs have changed.
    // By default, (i.e. if this line weren't here), all data IDs
    // would be bumped after the SOP cook, to indicate that
    // everything might have changed.
    // If some data IDs don't get bumped properly, the viewport
    // may not update, or SOPs that check data IDs
    // may not cook correctly, so be *very* careful!
    mySopFlags.setManagesDataIDs(true);
}

//populate our UT_String array with values from pt attribs
void SOP_groupDiff::getPointAttribs(UT_ValArray<UT_String>& arrin, const GA_Attribute* attr_in, const GU_Detail* inputGeo_in, bool debug)
{
    //try and figure out which input is coming in
    int input_number = 0;

    if(inputGeo_in->getUniqueId() == inputGeo(0)->getUniqueId())
    {
        if(debug)
            std::cout << "!!FOUND THE FIRST INPUT!!" << std::endl;

        input_number = 1;
    }
    else if(inputGeo_in->getUniqueId() == inputGeo(1)->getUniqueId())
    {
        if(debug)
            std::cout << "!!FOUND THE SECOND INPUT!!" << std::endl;

        input_number = 2;
    }
    else
    {
        std::cout << "Something went wrong, no input found" << std::endl;
    }

    //now iterate thru all thepoints and populate the array with attr string values
    for(GA_Iterator it(inputGeo_in->getPointRange()); !it.atEnd(); ++it)
    {
        GA_Offset offset = *it;

        GA_ROAttributeRef attr(attr_in);
        if(debug)
        {
            std::cout << "\n-----------------------------------------" << std::endl;
            std::cout << "===the value of input " << input_number << " attr is: " << attr.getString(offset) << std::endl;
        }
        arrin.append(attr.getString(offset));

    }
}

//out templated version of the population of our T array with values from pt attribs
template<typename T>
void SOP_groupDiff::getPointAttribs(UT_ValArray<T>& arrin, const GA_Attribute* attr_in, const GU_Detail* inputGeo_in, bool debug)
{
    //try and figure out which input is coming in
    int input_number = 0;

    if(inputGeo_in->getUniqueId() == inputGeo(0)->getUniqueId())
    {
        if(debug)
            std::cout << "!!FOUND THE FIRST INPUT!!" << std::endl;

        input_number = 1;
    }
    else if(inputGeo_in->getUniqueId() == inputGeo(1)->getUniqueId())
    {
        if(debug)
            std::cout << "!!FOUND THE SECOND INPUT!!" << std::endl;

        input_number = 2;
    }
    else
    {
        std::cout << "Something went wrong, no input found" << std::endl;
    }

    //now iterate thru all thepoints and populate the array with attr float/int values
    for(GA_Iterator it(inputGeo_in->getPointRange()); !it.atEnd(); ++it)
    {
        GA_Offset offset = *it;

        GA_ROHandleT<T> attr(attr_in);
        if(debug)
        {
            std::cout << "\n-----------------------------------------" << std::endl;
            std::cout << "===the value of input " << input_number << " attr is: " << attr.get(offset) << std::endl;
        }
        arrin.append(attr.get(offset));

    }
}

//compare our arrays
template <typename T>
void SOP_groupDiff::compareUTArrays(const UT_ValArray<T>& cmpr1, const UT_ValArray<T>& cmpr2, UT_ValArray<T>& newAr)
{

    //in case the first is larger than the second
    if(cmpr1.size() > cmpr2.size())
    {
        //iterate thru the first array
        for(typename UT_ValArray<T>::const_iterator itF = cmpr1.begin(); itF != cmpr1.end(); ++itF)
        {
            //setup local vars for flagging the difference between the two arrays
            bool found = false;
            int count = 0;
            //iterate thru the second array
            for(typename UT_ValArray<T>::const_iterator itS = cmpr2.begin(); itS != cmpr2.end(); ++itS)
            {
                //as long as you havent found two of the same
                if(!found)
                {
                    //if you have just found it
                    if(*itF == *itS)
                    {
                        if(debug)
                            std::cout << "just found that " << *itF << " is the same as " << *itS << std::endl;
                        //flag it and get the hell outta there
                        found = true;
                        count++;
                        continue;
                    }

                }
            }
            //if we havent found the same value in the other array
            if(count == 0)
            {
                //thats our difference, add it to the new array
                newAr.append(*itF);
            }
        }
    }
    else
    {
        //iterate thru the first array
        for(typename UT_ValArray<T>::const_iterator itF = cmpr2.begin(); itF != cmpr2.end(); ++itF)
        {
            //setup local vars for flagging the difference between the two arrays
            bool found = false;
            int count = 0;
            //iterate thru the second array
            for(typename UT_ValArray<T>::const_iterator itS = cmpr1.begin(); itS != cmpr1.end(); ++itS)
            {
                //as long as you havent found two of the same
                if(!found)
                {
                    //if you have just found it
                    if(*itF == *itS)
                    {
                        if(debug)
                            std::cout << "just found that " << *itF << " is the same as " << *itS << std::endl;
                        //flag it and get the hell outta there
                        found = true;
                        count++;
                        continue;
                    }

                }
            }
            //if we havent found the same value in the other array
            if(count == 0)
            {
                //thats our difference, add it to the new array
                newAr.append(*itF);
            }
        }
    }
}

//our UT_String method for grouping and deleting points
void SOP_groupDiff::groupOrDeletePts(const UT_String& groupName_in, int delToggle, const GA_Attribute* attr_in,
        const GU_Detail* secondInput_in, const UT_ValArray<UT_String>& arrin)
{
    //create our group diff ptr
    GA_PointGroup *diffGrp;
    //set the name of the group diff
    diffGrp = gdp->newPointGroup(groupName_in, false);
    //create our delete temp group
    GA_PointGroup *delGrp = gdp->newPointGroup("__tempDelGrp__", false);

    //read our attr_in handle
    GA_ROAttributeRef lookup_attr(attr_in);

    //template value that we can use to compare
    UT_String value;

    //now iterate thru all the points...
    for(GA_Iterator it(gdp->getPointRange()); !it.atEnd(); ++it)
    {
        //get the attribute string value
        GA_Offset offset = *it;
        value = lookup_attr.getString(offset);

        if(debug)
        {
            std::cout << "-----------------------------------------------------------------" << std::endl;
            std::cout << "-->point # " << offset << " with value of " << value << std::endl;
        }
        //...and start comparing the value with the input array values
        for(UT_ValArray<UT_String>::const_iterator itarr = arrin.begin(); itarr != arrin.end(); ++itarr)
        {
            if(debug)
                std::cout << "\tbeing compared to diff lists of value " << *itarr << std::endl;

            //if the two values are the same
            if(value == *itarr)
            {
                //add it to the group
                diffGrp->addOffset(offset);
                if(debug)
                    std::cout << "---> ADDED POINT: " << offset << std::endl;

                continue;
            }

        }
        if(debug)
            std::cout << "-----------------------------------------------------------------" << std::endl;

    }

    //the diff between the first and second input is found in the diff array
    //however we need to delete everything thats not the diff, and that is the second inputs members translated into first
    for(GA_Iterator it(secondInput_in->getPointRange()); !it.atEnd(); ++it)
    {
        GA_Offset offset = *it;
        delGrp->addOffset(offset);
    }

    //if the user chose to delete the pts rather than group them, do exactly that
    if(delToggle)
    {
        gdp->destroyPointOffsets(GA_Range(*delGrp));
        gdp->destroyGroup(delGrp);
    }
}


//our template method for grouping or deleting points
template <typename T>
void SOP_groupDiff::groupOrDeletePts(const UT_String& groupName_in, int delToggle, const GA_Attribute* attr_in,
        const GU_Detail* secondInput_in, const UT_ValArray<T>& arrin)
{
    //create our group diff ptr
    GA_PointGroup *diffGrp;
    //set the name of the group diff
    diffGrp = gdp->newPointGroup(groupName_in, false);
    //create our delete temp group
    GA_PointGroup *delGrp = gdp->newPointGroup("__tempDelGrp__", false);

    //read our attr_in handle
    GA_ROHandleT<T> lookup_attr(attr_in);

    //template value that we can use to compare
    T value;

    for(GA_Iterator it(gdp->getPointRange()); !it.atEnd(); ++it)
    {
        GA_Offset offset = *it;
        value = lookup_attr.get(offset);
        if(debug)
        {
            std::cout << "-----------------------------------------------------------------" << std::endl;
            std::cout << "-->point # " << offset << " with value of " << value << std::endl;
        }
        for(typename UT_ValArray<T>::const_iterator itarr = arrin.begin(); itarr != arrin.end(); ++itarr)
        {
            if(value == *itarr)
            {
                diffGrp->addOffset(offset);
                if(debug)
                    std::cout << "---> ADDED POINT: " << offset << std::endl;

                continue;
            }

        }

    }
    if(debug)
        std::cout << "-----------------------------------------------------------------" << std::endl;

    //the diff between the first and second input is found in the diff array
    //however we need to delete everything thats not the diff, and that is the second inputs members translated into first
    for(GA_Iterator it(secondInput_in->getPointRange()); !it.atEnd(); ++it)
    {
        GA_Offset offset = *it;
        delGrp->addOffset(offset);
    }

    //if the user chose to delete the pts rather than group them, do exactly that
    if(delToggle)
    {
        gdp->destroyPointOffsets(GA_Range(*delGrp));
        gdp->destroyGroup(delGrp);
    }
}

SOP_groupDiff::~SOP_groupDiff() {}

OP_ERROR
SOP_groupDiff::cookInputGroups(OP_Context &context, int alone)
{
    // The SOP_Node::cookInputPointGroups() provides a good default
    // implementation for just handling a point selection.
    return cookInputPointGroups(
        context, // This is needed for cooking the group parameter, and cooking the input if alone.
        myGroup, // The group (or NULL) is written to myGroup if not alone.
        alone,   // This is true iff called outside of cookMySop to update handles.
                 // true means the group will be for the input geometry.
                 // false means the group will be for gdp (the working/output geometry).
        true,    // (default) true means to set the selection to the group if not alone and the highlight flag is on.
        0,       // (default) Parameter index of the group field
        -1,      // (default) Parameter index of the group type field (-1 since there isn't one)
        true,    // (default) true means that a pointer to an existing group is okay; false means group is always new.
        false,   // (default) false means new groups should be unordered; true means new groups should be ordered.
        true,    // (default) true means that all new groups should be detached, so not owned by the detail;
                 //           false means that new point and primitive groups on gdp will be owned by gdp.
        0        // (default) Index of the input whose geometry the group will be made for if alone.
    );
}

OP_ERROR
SOP_groupDiff::cookMySop(OP_Context &context)
{
    // We must lock our inputs before we try to access their geometry.
    // OP_AutoLockInputs will automatically unlock our inputs when we return.
    // NOTE: Don't call unlockInputs yourself when using this!
    OP_AutoLockInputs inputs(this);
    if (inputs.lock(context) >= UT_ERROR_ABORT)
        return error();

    // Duplicate our incoming geometry with the hint that we only
    // altered points.  Thus, if our input was unchanged, we can
    // easily roll back our changes by copying point values.
    duplicatePointSource(0, context);

    fpreal t = context.getTime();

    // We evaluate our parameters outside the loop for speed.  If we
    // wanted local variable support, we'd have to do more setup
    // (see SOP_Flatten) and also move these inside the loop.
    UT_String lookup, grpName;
    int newGrp, deletePts;

    getAttr(lookup, t);
    newGrp = getGrpToggle(t);
    getGrpName(grpName, t);
    deletePts = getDelToggle(t);
//    useAttrib = getAttrib(t);
//    comparePos = getPosToggle(t);
    debug = getDebugToggle(t);

    //make sure attr is a valid string
    if(!lookup.isstring())
        return error();

    lookup.forceValidVariableName();

    //get ptrs to our input geo
    const GU_Detail *firstInput  = inputGeo(0);
    const GU_Detail *secondInput = inputGeo(1);

    //find if attr exist in those geos
    const GA_Attribute *fattr = firstInput->findAttribute(GA_ATTRIB_POINT, lookup);
    const GA_Attribute *sattr;

    //get our total numpts
    if(debug)
    {
        std::cout << "\n=====================GROUP DIFF SOP LOG ===============================================" << std::endl;
        std::cout << "total num of pts in inputGeo(1): " << firstInput->getPointMap().indexSize() << std::endl;
    }
    //if we have hooked a second input, find attrs and get total numpts
    if(secondInput)
    {
        sattr = secondInput->findAttribute(GA_ATTRIB_POINT, lookup);
        if(debug)
            std::cout << "total num of pts in inputGeo(2): " << secondInput->getPointMap().indexSize() << std::endl;
    }

    // If source attribute doesn't exist, error.
    if (!fattr || !sattr)
    {
        addError(SOP_ATTRIBUTE_INVALID, (const char *)lookup);
        return error();
    }

    if (error() >= UT_ERROR_ABORT)
        return error();

    // Here we determine which groups we have to work on.  We only
    // handle point groups.
    if (cookInputGroups(context) >= UT_ERROR_ABORT)
        return error();


    if(fattr->getStorageClass() == GA_STORECLASS_FLOAT || fattr->getStorageClass() == GA_STORECLASS_REAL)
    {
        UT_ValArray<fpreal64>(ptNumListDiff);
        UT_ValArray<fpreal64>(ptNumListFirst);
        UT_ValArray<fpreal64>(ptNumListSecond);

        if(debug)
        {
            std::cout << "\n-----------------------------------------" << std::endl;
            std::cout << "The attribute you have chosen is a float!" << std::endl;
        }
        //feed our list of attr values in the array FIRST GEO
//        gdp->getPointAttributeAsArray(fattr,gdp->getPointRange(), ptNumListFirst);
        getPointAttribs(ptNumListFirst, fattr, firstInput, debug);

        //if we have connected the second geo
        if(secondInput)
        {
            //feed our list of attr values in the array SECOND GEO
//            gdp->getPointAttributeAsArray(sattr,gdp->getPointRange(), ptNumListSecond);
            getPointAttribs(ptNumListSecond, sattr, secondInput, debug);

            //compare our first and second input list and feed the diff in the third input
            compareUTArrays(ptNumListFirst, ptNumListSecond, ptNumListDiff);

            if(debug)
                printArrayVal(ptNumListDiff);
        }

        //if we ticked new group, create a new group for pts
        if(newGrp || deletePts)
        {
            //our function that deals with grouping or deleting
            groupOrDeletePts<fpreal64>(grpName, deletePts, fattr, secondInput, ptNumListDiff);

        }
    }
    else if(fattr->getStorageClass() == GA_STORECLASS_INT)
    {
        if(debug)
        {
            std::cout << "\n-----------------------------------------" << std::endl;
            std::cout << "The attribute you have chosen is a int!" << std::endl;
        }

        //feed our list of attr values in the array FIRST GEO
//        gdp->getPointAttributeAsArray(fattr,gdp->getPointRange(), ptNumListFirst);
        getPointAttribs(ptNumListFirst, fattr, firstInput, debug);

        //if we have connected the second geo
        if(secondInput)
        {
            //feed our list of attr values in the array SECOND GEO
//            gdp->getPointAttributeAsArray(sattr,gdp->getPointRange(), ptNumListSecond);
            getPointAttribs(ptNumListSecond, sattr, secondInput, debug);

            //compare our first and second input list and feed the diff in the third input
            compareUTArrays(ptNumListFirst, ptNumListSecond, ptNumListDiff);
        }

        //if we ticked new group, create a new group for pts
        if(newGrp || deletePts)
        {
            //our function that deals with grouping or deleting
            groupOrDeletePts<int32>(grpName, deletePts, fattr, secondInput, ptNumListDiff);

        }

    }
    else if(fattr->getStorageClass() == GA_STORECLASS_STRING)
    {
        UT_ValArray<UT_String>(ptNumListDiff);
        UT_ValArray<UT_String>(ptNumListFirst);
        UT_ValArray<UT_String>(ptNumListSecond);

        if(debug)
        {
            std::cout << "\n-----------------------------------------" << std::endl;
            std::cout << "The attribute you have chosen is a string!" << std::endl;
        }

        //feed our list of attr values in the array FIRST GEO
        getPointAttribs(ptNumListFirst, fattr, firstInput, debug);

        //if we have connected the second input
        if(secondInput)
        {
            //get the values and feed it into our list
            getPointAttribs(ptNumListSecond, sattr, secondInput, debug);

            //compare our first and second input list and feed the diff in the third input
            compareUTArrays(ptNumListFirst, ptNumListSecond, ptNumListDiff);
        }

        //if we ticked new group, create a new group for pts
        if(newGrp || deletePts)
        {
            //our function that deals with grouping or deleting
            groupOrDeletePts(grpName, deletePts, fattr, secondInput, ptNumListDiff);

        }
    }
    else
    {
        addError(SOP_ATTRIBUTE_INVALID, (const char *)lookup);
        return error();
    }


//    GA_Offset ptoff;
//    GA_FOR_ALL_GROUP_PTOFF(gdp, myGroup, ptoff)
//    {
//        //get pts id
//        GA_Index ptindx = gdp->pointIndex(ptoff);
//
////        ptNumList.append(gdp->getAttributeLayer(lookup));
//        std::cout <<  << std::endl;
////        if(secondInput)
////            std::cout << secondInput->pointIndex(ptoff) << std::endl;
////        //        UT_Vector3 p = gdp->getPos3(ptoff);
////
//////        p.y() += SYSsin( (p.x() / period + phase) * M_PI * 2 ) * amp;
////
//////        gdp->setPos3(ptoff, p);
//    }

    // If we've modified P, and we're managing our own data IDs,
    // we must bump the data ID for P.
    if (!myGroup || !myGroup->isEmpty())
        gdp->getP()->bumpDataId();

    return error();
}

template<typename T>
void SOP_groupDiff::printArrayVal(const UT_ValArray<T>& arrIn)
{
    for(typename UT_ValArray<T>::const_iterator it = arrIn.begin(); it != arrIn.end(); ++it)
    {
        std::cout << "Value of array is: " << *it << std::endl;
    }
}
