//
//  tbzScreenScale.cpp
//  IMCFestivalApp
//
//  Created by TBZ.PhD on 21/09/2012.
//
//

#include "tbzScreenScale.h"

// This simple functionality is done like this so we can compile as obj-c++

float tbzScreenScale::retinaScale = 1;
void tbzScreenScale::detectScale()
{
    if ([UIScreen instancesRespondToSelector:@selector(scale)])
    {
        retinaScale = [[UIScreen mainScreen] scale];
    }
}
