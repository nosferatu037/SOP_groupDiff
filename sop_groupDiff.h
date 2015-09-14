#ifndef SOP_GROUPASATTR_H
#define SOP_GROUPASATTR_H

//#include "hdk_plugins_global.h"

/*
 * File:   SOP_groupDiff.h
 * Author: nosferatu
 *
 * Created on August 29, 2015, 1:22 PM
 */

//#ifndef SOP_GROUPDIFF_H
//#define	SOP_GROUPDIFF_H

#include <SOP/SOP_Node.h>

namespace HDK_Sample
{
//    template<typename T>

    class SOP_groupDiff : public SOP_Node
    {
    public:
        SOP_groupDiff(OP_Network *net, const char *name, OP_Operator *op);
        virtual ~SOP_groupDiff();


        static PRM_Template myTemplateList[];
        static OP_Node  *myCtor(OP_Network*, const char*, OP_Operator*);
        /// This method is created so that it can be called by handles.  It only
        /// cooks the input group of this SOP.  The geometry in this group is
        /// the only geometry manipulated by this SOP.
        virtual OP_ERROR    cookInputGroups(OP_Context &context, int alone = 0);

    protected:
        /// Method to cook geometry for the SOP
        virtual OP_ERROR    cookMySop(OP_Context &context);

    private:

        //get our attribute and store it in the str var
        void    getAttr(UT_String &str, fpreal t)
        {
            evalString(str, "attrLook", 0, t);
        }
        //get our toggle create groups
        int    getGrpToggle(fpreal t)
        {
            return evalInt("createGrp", 0, t);
        }
        //get our grp name
        void    getGrpName(UT_String &str, fpreal t)
        {
            evalString(str, "newGrp", 0, t);
        }
        //get our delete pts toggle
        int    getDelToggle(fpreal t)
        {
            return evalInt("delPts", 0, t);
        }
//        int getAttrib(fpreal t)
//        {
//            return evalInt("useAttr", 0, t);
//        }
//        //get our P flag
//        int getPosToggle(fpreal t)
//        {
//            return evalInt("getPos", 0, t);
//        }
        //get our debug flag
        int getDebugToggle(fpreal t)
        {
            return evalInt("debug", 0, t);
        }


        template <typename T>
        void printArrayVal(const UT_ValArray<T>&arrIn);

        //create a template method for the arrays
        template <typename T>
        void    compareUTArrays(const UT_ValArray<T>&cmpr1, const UT_ValArray<T>&cmpr2, UT_ValArray<T>&newAr);

        template <typename T>
        void    groupOrDeletePts(const UT_String& groupName_in, int delToggle, const GA_Attribute* attr_in, const GU_Detail* secondInput_in, const UT_ValArray<T>& arrin);

        void    groupOrDeletePts(const UT_String& groupName_in, int delToggle, const GA_Attribute* attr_in, const GU_Detail* secondInput_in, const UT_ValArray<UT_String>& arrin);
//        void    groupOrDeletePtsI(const UT_String& groupName_in, int delToggle, const GA_Attribute* attr_in, const GU_Detail* secondInput_in);
//        void    groupOrDeletePtsS(const UT_String& groupName_in, int delToggle, const GA_Attribute* attr_in, const GU_Detail* secondInput_in);


        void    getPointAttribs(UT_ValArray<UT_String>& arrin, const GA_Attribute* attr_in, const GU_Detail* inputGeo_in, bool debug = false);

        template <typename T>
        void    getPointAttribs(UT_ValArray<T>& arrin, const GA_Attribute* attr_in, const GU_Detail* inputGeo_in, bool debug = false);

        //define our arrays
        UT_ValArray<int32> ptNumListFirst;
        UT_ValArray<int32> ptNumListSecond;
        UT_ValArray<int32> ptNumListDiff;

        int debug;

        //        void disableParms(OP_Node *, OP_Context &context);

//        /// This variable is used together with the call to the "checkInputChanged"
//        /// routine to notify the handles (if any) if the input has changed.
//        GU_DetailGroupPair	 myDetailGroupPair;

        /// This is the group of geometry to be manipulated by this SOP and cooked
        /// by the method "cookInputGroups".
        const GA_PointGroup	*myGroup;
        // End HDK_Sample namespa
    };
}

//#endif	/* SOP_GROUPDIFF_H */


#endif // SOP_GROUPASATTR_H
