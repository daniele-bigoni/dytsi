/*
 * This file is part of DYTSI.
 *
 * DYTSI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DYTSI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DYTSI.  If not, see <http://www.gnu.org/licenses/>.
 *
 * DYnamics Train SImulation (DYTSI)
 * Copyright (C) 2011-2014 The Technical University of Denmark
 * Scientific Computing Section
 * Department of Applied Mathematics and Computer Science
 *
 * Author: Daniele Bigoni
 * E-mail: dabi@dtu.dk
 *
 */

/* 
 * File:   CooperriderOrthogonalModel.h
 * Author: bigo
 *
 * Created on April 14, 2011, 7:08 PM
 */

#ifndef COOPERRIDERORTHOGONALMODEL_H
#define	COOPERRIDERORTHOGONALMODEL_H
#include "../GeneralModel/DYTSI_Modelling.h"
#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

namespace DYTSI_Models {
    class CooperriderOrthogonalModel : public DYTSI_Modelling::GeneralModel {
    public:
        CooperriderOrthogonalModel();
        virtual ~CooperriderOrthogonalModel();

        std::vector<DYTSI_Modelling::Component*> getComponentList();

        double getD1() const;
        double getD2() const;
        double getD3() const;
        double getD4() const;
        double getD5() const;
        double getD6() const;
        double getK1() const;
        double getK2() const;
        double getK3() const;
        double getK4() const;
        double getK5() const;
        double getK6() const;
        double getR0() const;
        double getH1() const;
        double getH2() const;
        double getA() const;
        double getL1() const;
        double getL2() const;
        double getL3() const;
        double getV1() const;
        double getV2() const;
        double getV3() const;
        double getV4() const;
        double getU1() const;
        double getU2() const;
        double getX1() const;
        double getS1() const;
        double getS2() const;
        double getS3() const;
        double getS4() const;

        XERCES_CPP_NAMESPACE::DOMNode* getRSGEO_path() const;

    private:
        typedef DYTSI_Modelling::GeneralModel super;

        void setRSGEO_path(XERCES_CPP_NAMESPACE::DOMNode* RSGEO_Path);
        void setD1(double D1);
        void setD2(double D2);
        void setD3(double D3);
        void setD4(double D4);
        void setD5(double D5);
        void setD6(double D6);
        void setK6(double k6);
        void setK5(double k5);
        void setK4(double k4);
        void setK3(double k3);
        void setK2(double k2);
        void setK1(double k1);
        void setR0(double val);
        void setH1(double val);
        void setH2(double val);
        void setA(double val);
        void setL1(double val);
        void setL2(double val);
        void setL3(double val);
        void setV1(double val);
        void setV2(double val);
        void setV3(double val);
        void setV4(double val);
        void setU1(double val);
        void setU2(double val);
        void setX1(double val);
        void setS1(double val);
        void setS2(double val);
        void setS3(double val);
        void setS4(double val);

        bool loadProperties(XERCES_CPP_NAMESPACE::DOMElement* el);

        XERCES_CPP_NAMESPACE::DOMNode* pathRSGEO;
        double k1;
        double k2;
        double k3;
        double k4;
        double k5;
        double k6;
        double D1;
        double D2;
        double D3;
        double D4;
        double D5;
        double D6;
        double r0;
        double h1;
        double h2;
        double a;
        double l1;
        double l2;
        double l3;
        double v1;
        double v2;
        double v3;
        double v4;
        double u1;
        double u2;
        double x1;
        double s1;
        double s2;
        double s3;
        double s4;

    protected:
        bool setupModel(XERCES_CPP_NAMESPACE::DOMElement* el);
        bool setupStaticLoads();
        bool loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues);

    };
}

#endif	/* COOPERRIDERORTHOGONALMODEL_H */

