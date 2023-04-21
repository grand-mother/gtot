//
// Created by lewhoo on 13/03/23.
//

#ifndef GTOT_LINKDEF_H
#define GTOT_LINKDEF_H

//#ifdef __ROOTCLING__
#pragma link C++ class vector < vector < vector<float> > > +;
#pragma link C++ class vector < vector < vector<short> > > +;
#pragma link C++ class vector < vector<unsigned short> > +;
#pragma link C++ class vector < vector<short> > +;
#pragma link C++ class vector < vector<float> > +;
#pragma link C++ class vector < vector<char> > +;
#pragma link C++ class vector < vector<bool> > +;
//#endif

#endif //GTOT_LINKDEF_H
