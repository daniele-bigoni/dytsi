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
 * CooperriderOrthogonalAsymmetricModel.h
 *
 *  Created on: Dec 8, 2012
 *      Author: dabi
 */

#ifndef COOPERRIDERORTHOASYMFIXEDMODEL_H
#define COOPERRIDERORTHOASYMFIXEDMODEL_H
#include "../GeneralModel/DYTSI_Modelling.h"
#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>

namespace DYTSI_Models {
	class CooperriderOrthoAsymFixedModel: public DYTSI_Modelling::GeneralModel {
	public:
		CooperriderOrthoAsymFixedModel();
		virtual ~CooperriderOrthoAsymFixedModel();

		std::vector<DYTSI_Modelling::Component*> getComponentList();

		double get_PSLL_LEFT_K1() const;
		double get_PSLL_LEFT_K2() const;
		double get_PSLL_LEFT_K3() const;
		double get_PSLL_RIGHT_K1() const;
		double get_PSLL_RIGHT_K2() const;
		double get_PSLL_RIGHT_K3() const;

		double get_PSLT_LEFT_K1() const;
		double get_PSLT_LEFT_K2() const;
		double get_PSLT_LEFT_K3() const;
		double get_PSLT_RIGHT_K1() const;
		double get_PSLT_RIGHT_K2() const;
		double get_PSLT_RIGHT_K3() const;

		double get_PSTL_LEFT_K1() const;
		double get_PSTL_LEFT_K2() const;
		double get_PSTL_LEFT_K3() const;
		double get_PSTL_RIGHT_K1() const;
		double get_PSTL_RIGHT_K2() const;
		double get_PSTL_RIGHT_K3() const;

		double get_PSTT_LEFT_K1() const;
		double get_PSTT_LEFT_K2() const;
		double get_PSTT_LEFT_K3() const;
		double get_PSTT_RIGHT_K1() const;
		double get_PSTT_RIGHT_K2() const;
		double get_PSTT_RIGHT_K3() const;

		double get_SSL_LEFT_K4() const;
		double get_SSL_LEFT_K5() const;
		double get_SSL_LEFT_K6() const;
		double get_SSL_LEFT_D1() const;
		double get_SSL_LEFT_D2() const;
		double get_SSL_LEFT_D6() const;
		double get_SSL_RIGHT_K4() const;
		double get_SSL_RIGHT_K5() const;
		double get_SSL_RIGHT_K6() const;
		double get_SSL_RIGHT_D1() const;
		double get_SSL_RIGHT_D2() const;
		double get_SSL_RIGHT_D6() const;

		double get_SST_LEFT_K4() const;
		double get_SST_LEFT_K5() const;
		double get_SST_LEFT_K6() const;
		double get_SST_LEFT_D1() const;
		double get_SST_LEFT_D2() const;
		double get_SST_LEFT_D6() const;
		double get_SST_RIGHT_K4() const;
		double get_SST_RIGHT_K5() const;
		double get_SST_RIGHT_K6() const;
		double get_SST_RIGHT_D1() const;
		double get_SST_RIGHT_D2() const;
		double get_SST_RIGHT_D6() const;

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

		void set_PSLL_LEFT_K1(double val);
		void set_PSLL_LEFT_K2(double val);
		void set_PSLL_LEFT_K3(double val);
		void set_PSLL_RIGHT_K1(double val);
		void set_PSLL_RIGHT_K2(double val);
		void set_PSLL_RIGHT_K3(double val);

		void set_PSLT_LEFT_K1(double val);
		void set_PSLT_LEFT_K2(double val);
		void set_PSLT_LEFT_K3(double val);
		void set_PSLT_RIGHT_K1(double val);
		void set_PSLT_RIGHT_K2(double val);
		void set_PSLT_RIGHT_K3(double val);

		void set_PSTL_LEFT_K1(double val);
		void set_PSTL_LEFT_K2(double val);
		void set_PSTL_LEFT_K3(double val);
		void set_PSTL_RIGHT_K1(double val);
		void set_PSTL_RIGHT_K2(double val);
		void set_PSTL_RIGHT_K3(double val);

		void set_PSTT_LEFT_K1(double val);
		void set_PSTT_LEFT_K2(double val);
		void set_PSTT_LEFT_K3(double val);
		void set_PSTT_RIGHT_K1(double val);
		void set_PSTT_RIGHT_K2(double val);
		void set_PSTT_RIGHT_K3(double val);

		void set_SSL_LEFT_K4(double val);
		void set_SSL_LEFT_K5(double val);
		void set_SSL_LEFT_K6(double val);
		void set_SSL_LEFT_D1(double val);
		void set_SSL_LEFT_D2(double val);
		void set_SSL_LEFT_D6(double val);
		void set_SSL_RIGHT_K4(double val);
		void set_SSL_RIGHT_K5(double val);
		void set_SSL_RIGHT_K6(double val);
		void set_SSL_RIGHT_D1(double val);
		void set_SSL_RIGHT_D2(double val);
		void set_SSL_RIGHT_D6(double val);

		void set_SST_LEFT_K4(double val);
		void set_SST_LEFT_K5(double val);
		void set_SST_LEFT_K6(double val);
		void set_SST_LEFT_D1(double val);
		void set_SST_LEFT_D2(double val);
		void set_SST_LEFT_D6(double val);
		void set_SST_RIGHT_K4(double val);
		void set_SST_RIGHT_K5(double val);
		void set_SST_RIGHT_K6(double val);
		void set_SST_RIGHT_D1(double val);
		void set_SST_RIGHT_D2(double val);
		void set_SST_RIGHT_D6(double val);

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

		double PSLL_LEFT_K1;
		double PSLL_LEFT_K2;
		double PSLL_LEFT_K3;
		double PSLL_RIGHT_K1;
		double PSLL_RIGHT_K2;
		double PSLL_RIGHT_K3;

		double PSLT_LEFT_K1;
		double PSLT_LEFT_K2;
		double PSLT_LEFT_K3;
		double PSLT_RIGHT_K1;
		double PSLT_RIGHT_K2;
		double PSLT_RIGHT_K3;

		double PSTL_LEFT_K1;
		double PSTL_LEFT_K2;
		double PSTL_LEFT_K3;
		double PSTL_RIGHT_K1;
		double PSTL_RIGHT_K2;
		double PSTL_RIGHT_K3;

		double PSTT_LEFT_K1;
		double PSTT_LEFT_K2;
		double PSTT_LEFT_K3;
		double PSTT_RIGHT_K1;
		double PSTT_RIGHT_K2;
		double PSTT_RIGHT_K3;

		double SSL_LEFT_K4;
		double SSL_LEFT_K5;
		double SSL_LEFT_K6;
		double SSL_LEFT_D1;
		double SSL_LEFT_D2;
		double SSL_LEFT_D6;
		double SSL_RIGHT_K4;
		double SSL_RIGHT_K5;
		double SSL_RIGHT_K6;
		double SSL_RIGHT_D1;
		double SSL_RIGHT_D2;
		double SSL_RIGHT_D6;

		double SST_LEFT_K4;
		double SST_LEFT_K5;
		double SST_LEFT_K6;
		double SST_LEFT_D1;
		double SST_LEFT_D2;
		double SST_LEFT_D6;
		double SST_RIGHT_K4;
		double SST_RIGHT_K5;
		double SST_RIGHT_K6;
		double SST_RIGHT_D1;
		double SST_RIGHT_D2;
		double SST_RIGHT_D6;

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

#endif /* COOPERRIDERORTHOASYMFIXEDMODEL_H */
