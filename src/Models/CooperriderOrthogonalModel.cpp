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
 * E-mail: dabi@limitcycle.it
 *
 */

/* 
 * File:   CooperriderOrthogonalModel.cpp
 * Author: bigo
 * 
 * Created on April 14, 2011, 7:08 PM
 */

#include "CooperriderOrthogonalModel.h"

#include "../Application/Input/XMLaux.h"
#include "../Application/Input/RSGEOTableReader.h"
#include "../Application/PrintingHandler.h"

#include <gsl/gsl_vector.h>

#include <sstream>

namespace DYTSI_Models {
    const double g = 9.81;

    CooperriderOrthogonalModel::CooperriderOrthogonalModel()
    : DYTSI_Modelling::GeneralModel(){
    }

    CooperriderOrthogonalModel::~CooperriderOrthogonalModel() {
    }

    std::vector<DYTSI_Modelling::Component*> CooperriderOrthogonalModel::getComponentList(){
        std::vector<DYTSI_Modelling::Component*> out;

        // Add the Root Component (Car Body)
        DYTSI_Modelling::Component* carBodyComponent = this->getRootComponent();
        out.push_back(carBodyComponent);

        // Add the Leading Bogie Frame
        DYTSI_Modelling::Component* leadingBogieFrameComponent = carBodyComponent->getLowerLeading()->getComponent();
        out.push_back(leadingBogieFrameComponent);

        // Add the Trailing Bogie Frame
        DYTSI_Modelling::Component* trailingBogieFrameComponent = carBodyComponent->getLowerTrailing()->getComponent();
        out.push_back(trailingBogieFrameComponent);

        // Add the Leading Wheelset in the Leading Bogie Frame
        DYTSI_Modelling::Component* llWheelSetComponent = leadingBogieFrameComponent->getLowerLeading()->getComponent();
        out.push_back(llWheelSetComponent);

        // Add the Trailing Wheelset in the Leading Bogie Frame
        DYTSI_Modelling::Component* ltWheelSetComponent = leadingBogieFrameComponent->getLowerTrailing()->getComponent();
        out.push_back(ltWheelSetComponent);

        // Add the Leading Wheelset in the Trailing Bogie Frame
        DYTSI_Modelling::Component* tlWheelSetComponent = trailingBogieFrameComponent->getLowerLeading()->getComponent();
        out.push_back(tlWheelSetComponent);

        // Add the Trailing Wheelset in the Trailing Bogie Frame
        DYTSI_Modelling::Component* ttWheelSetComponent = trailingBogieFrameComponent->getLowerTrailing()->getComponent();
        out.push_back(ttWheelSetComponent);

        return out;
    }

    bool CooperriderOrthogonalModel::loadProperties(XERCES_CPP_NAMESPACE::DOMElement* el){
        bool errCode = false;

        // Set RSGEO path
        errCode = XMLaux::loadRSGEOProperty(el,"RSGEO_Path",this->pathRSGEO);
        if (errCode) return errCode;

        // Set r0
        errCode = XMLaux::loadDoubleProperty(el,"r0",this->r0);
        if (errCode) return errCode;

        // Set h1
        errCode = XMLaux::loadDoubleProperty(el,"h1",this->h1);
        if (errCode) return errCode;

        // Set h2
        errCode = XMLaux::loadDoubleProperty(el,"h2",this->h2);
        if (errCode) return errCode;

        // Set a
        errCode = XMLaux::loadDoubleProperty(el,"a",this->a);
        if (errCode) return errCode;

        // Set l1
        errCode = XMLaux::loadDoubleProperty(el,"l1",this->l1);
        if (errCode) return errCode;

        // Set l2
        errCode = XMLaux::loadDoubleProperty(el,"l2",this->l2);
        if (errCode) return errCode;

        // Set l3
        errCode = XMLaux::loadDoubleProperty(el,"l3",this->l3);
        if (errCode) return errCode;

        // Set v1
        errCode = XMLaux::loadDoubleProperty(el,"v1",this->v1);
        if (errCode) return errCode;

        // Set v2
        errCode = XMLaux::loadDoubleProperty(el,"v2",this->v2);
        if (errCode) return errCode;

        // Set v3
        errCode = XMLaux::loadDoubleProperty(el,"v3",this->v3);
        if (errCode) return errCode;

        // Set v4
        errCode = XMLaux::loadDoubleProperty(el,"v4",this->v4);
        if (errCode) return errCode;

        // Set u1
        errCode = XMLaux::loadDoubleProperty(el,"u1",this->u1);
        if (errCode) return errCode;

        // Set u2
        errCode = XMLaux::loadDoubleProperty(el,"u2",this->u2);
        if (errCode) return errCode;

        // Set x1
        errCode = XMLaux::loadDoubleProperty(el,"x1",this->x1);
        if (errCode) return errCode;

        // Set s1
        errCode = XMLaux::loadDoubleProperty(el,"s1",this->s1);
        if (errCode) return errCode;

        // Set s2
        errCode = XMLaux::loadDoubleProperty(el,"s2",this->s2);
        if (errCode) return errCode;

        // Set s3
        errCode = XMLaux::loadDoubleProperty(el,"s3",this->s3);
        if (errCode) return errCode;

        // Set s4
        errCode = XMLaux::loadDoubleProperty(el,"s4",this->s4);
        if (errCode) return errCode;

        // Set k1
        errCode = XMLaux::loadDoubleProperty(el,"k1",this->k1);
        if (errCode) return errCode;

        // Set k2
        errCode = XMLaux::loadDoubleProperty(el,"k2",this->k2);
        if (errCode) return errCode;

        // Set k3
        errCode = XMLaux::loadDoubleProperty(el,"k3",this->k3);
        if (errCode) return errCode;

        // Set k4
        errCode = XMLaux::loadDoubleProperty(el,"k4",this->k4);
        if (errCode) return errCode;

        // Set k5
        errCode = XMLaux::loadDoubleProperty(el,"k5",this->k5);
        if (errCode) return errCode;

        // Set k6
        errCode = XMLaux::loadDoubleProperty(el,"k6",this->k6);
        if (errCode) return errCode;

        // Set D1
        errCode = XMLaux::loadDoubleProperty(el,"D1",this->D1);
        if (errCode) return errCode;

        // Set D2
        errCode = XMLaux::loadDoubleProperty(el,"D2",this->D2);
        if (errCode) return errCode;

        // Set D3
        errCode = XMLaux::loadDoubleProperty(el,"D3",this->D3);
        if (errCode) return errCode;

        // Set D4
        errCode = XMLaux::loadDoubleProperty(el,"D4",this->D4);
        if (errCode) return errCode;

        // Set D5
        errCode = XMLaux::loadDoubleProperty(el,"D5",this->D5);
        if (errCode) return errCode;

        // Set D6
        errCode = XMLaux::loadDoubleProperty(el,"D6",this->D6);
        if (errCode) return errCode;

        return errCode;
    }

    bool CooperriderOrthogonalModel::loadStartingValues(XERCES_CPP_NAMESPACE::DOMElement* el, std::vector<double>& startingValues){
        bool error = false;

        if (this->getSettedUp() == false){
            error = true;
        } else {
            // Set the y0 vector to the correct size
            startingValues.resize(this->get_N_DOF());

            // Set Starting values for the CarBody root component
            DYTSI_Modelling::CarBodyComponent* carBody = (DYTSI_Modelling::CarBodyComponent*)this->getRootComponent();
            XERCES_CPP_NAMESPACE::DOMElement* carBodySV;
            error = XMLaux::loadElementProperty(el, "CarBody", carBodySV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* carBodyValues;
            error = XMLaux::loadElementProperty(carBodySV, "CarBodyValues", carBodyValues);
            if (error) return error;
            carBody->loadStartingValues(carBodyValues, startingValues);

            // Set Starting values for the Leading Bogieframe
            DYTSI_Modelling::BogieFrameComponent* leadingBogieFrame =
                    (DYTSI_Modelling::BogieFrameComponent*)carBody->getLowerLeading()->getComponent();
            XERCES_CPP_NAMESPACE::DOMElement* leadingBogieFrameSV;
            error = XMLaux::loadElementProperty(carBodySV, "LeadingBogieFrame", leadingBogieFrameSV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* leadingBogieFrameValues;
            error = XMLaux::loadElementProperty(leadingBogieFrameSV, "LeadingBogieFrameValues", leadingBogieFrameValues);
            if (error) return error;
            leadingBogieFrame->loadStartingValues(leadingBogieFrameValues, startingValues);

            // Set Starting values for the Leading Wheelset in the Leading bogie frame
            DYTSI_Modelling::WheelSetComponent* llWheelSet =
                    (DYTSI_Modelling::WheelSetComponent*)leadingBogieFrame->getLowerLeading()->getComponent();
            XERCES_CPP_NAMESPACE::DOMElement* llWheelSetSV;
            error = XMLaux::loadElementProperty(leadingBogieFrameSV, "LeadingWheelSet", llWheelSetSV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* llWheelSetValues;
            error = XMLaux::loadElementProperty(llWheelSetSV, "LLWheelSetValues", llWheelSetValues);
            if (error) return error;
            llWheelSet->loadStartingValues(llWheelSetValues, startingValues);

            // Set Starting values for the Trailing Wheelset in the Leading bogie frame
            DYTSI_Modelling::WheelSetComponent* ltWheelSet =
                    (DYTSI_Modelling::WheelSetComponent*)leadingBogieFrame->getLowerTrailing()->getComponent();
            XERCES_CPP_NAMESPACE::DOMElement* ltWheelSetSV;
            error = XMLaux::loadElementProperty(leadingBogieFrameSV, "TrailingWheelSet", ltWheelSetSV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* ltWheelSetValues;
            error = XMLaux::loadElementProperty(ltWheelSetSV, "LTWheelSetValues", ltWheelSetValues);
            if (error) return error;
            ltWheelSet->loadStartingValues(ltWheelSetValues, startingValues);

            // Set Starting values for the Trailing Bogieframe
            DYTSI_Modelling::BogieFrameComponent* trailingBogieFrame =
                    (DYTSI_Modelling::BogieFrameComponent*)carBody->getLowerTrailing()->getComponent();
            XERCES_CPP_NAMESPACE::DOMElement* trailingBogieFrameSV;
            error = XMLaux::loadElementProperty(carBodySV, "TrailingBogieFrame", trailingBogieFrameSV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* trailingBogieFrameValues;
            error = XMLaux::loadElementProperty(trailingBogieFrameSV, "TrailingBogieFrameValues", trailingBogieFrameValues);
            if (error) return error;
            trailingBogieFrame->loadStartingValues(trailingBogieFrameValues, startingValues);

            // Set Starting values for the Leading Wheelset in the Trailing bogie frame
            DYTSI_Modelling::WheelSetComponent* tlWheelSet =
                    (DYTSI_Modelling::WheelSetComponent*)trailingBogieFrame->getLowerLeading()->getComponent();
            XERCES_CPP_NAMESPACE::DOMElement* tlWheelSetSV;
            error = XMLaux::loadElementProperty(trailingBogieFrameSV, "LeadingWheelSet", tlWheelSetSV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* tlWheelSetValues;
            error = XMLaux::loadElementProperty(tlWheelSetSV, "TLWheelSetValues", tlWheelSetValues);
            if (error) return error;
            tlWheelSet->loadStartingValues(tlWheelSetValues, startingValues);

            // Set Starting values for the Trailing Wheelset in the Trailing bogie frame
            DYTSI_Modelling::WheelSetComponent* ttWheelSet =
                    (DYTSI_Modelling::WheelSetComponent*)trailingBogieFrame->getLowerTrailing()->getComponent();
            XERCES_CPP_NAMESPACE::DOMElement* ttWheelSetSV;
            error = XMLaux::loadElementProperty(trailingBogieFrameSV, "TrailingWheelSet", ttWheelSetSV);
            if (error) return error;
            XERCES_CPP_NAMESPACE::DOMElement* ttWheelSetValues;
            error = XMLaux::loadElementProperty(ttWheelSetSV, "TTWheelSetValues", ttWheelSetValues);
            if (error) return error;
            ttWheelSet->loadStartingValues(ttWheelSetValues, startingValues);
        }

        return error;
    }

    bool CooperriderOrthogonalModel::setupModel( XERCES_CPP_NAMESPACE::DOMElement* el ){
        bool error = this->loadProperties(el);
        if ( error ) return error;

        DYTSI_Modelling::CarBodyComponent *carBody;
        DYTSI_Modelling::BogieFrameComponent *leadingBogieFrame;
        DYTSI_Modelling::BogieFrameComponent *trailingBogieFrame;

        // Setup the Car Body as root Component
        // TODO: Retreive real measures
        {
            double mass = 44388.0;
            double Ix = 2.80e5;
            double Iy = 5.0e5;
            double Iz = 5.0e5;
            gsl_vector* centerOfGeometry = gsl_vector_calloc(3);
            gsl_vector_set(centerOfGeometry, 2, r0+h1+h2);
            gsl_vector* centerOfMass = gsl_vector_calloc(3);
            carBody = new DYTSI_Modelling::CarBodyComponent( centerOfGeometry, centerOfMass,
                    mass,Ix,Iy,Iz,this->getExternalComponents(),
                    "Car Body", this);
            this->setRootComponent( carBody );
        };

        // Setup the Bogieframes, the connection and add them to the model
        {
            double mass = 2918.9;
            double Ix = 6780.0;
            double Iy = 6780.0;
            double Iz = 6780.0;
            gsl_vector* centerOfGeometryLeading = gsl_vector_calloc(3);
            gsl_vector_set(centerOfGeometryLeading, 0, u1);
            gsl_vector_set(centerOfGeometryLeading, 2, r0+h1 );
            gsl_vector* centerOfMassLeading = gsl_vector_calloc(3);
            leadingBogieFrame = new DYTSI_Modelling::BogieFrameComponent(
                    DYTSI_Modelling::LEADING, centerOfGeometryLeading, centerOfMassLeading,
                    mass, Ix, Iy, Iz, u2,
                    this->getExternalComponents(),
                    "Leading Bogie Frame", this );

            gsl_vector* centerOfGeometryTrailing = gsl_vector_calloc(3);
            gsl_vector_set(centerOfGeometryTrailing, 0, -u1);
            gsl_vector_set(centerOfGeometryTrailing, 2, r0+h1 );
            gsl_vector* centerOfMassTrailing = gsl_vector_calloc(3);
            trailingBogieFrame = new DYTSI_Modelling::BogieFrameComponent(
                    DYTSI_Modelling::TRAILING, centerOfGeometryTrailing, centerOfMassTrailing,
                    mass, Ix, Iy, Iz, u2,
                    this->getExternalComponents(),
                    "Trailing Bogie Frame", this);

            //////////////////////////////
            // Secondary Suspensions Leading bogie frame
            DYTSI_Modelling::BogieFrameCarBodyConnector *connectorLeading =
                    new DYTSI_Modelling::BogieFrameCarBodyConnector(
                    "Secondary Suspensions Leading bogie frame", carBody,
                    leadingBogieFrame, DYTSI_Modelling::UP, DYTSI_Modelling::LL);

            // Left lateral spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u1);
                gsl_vector_set(connectionPointUp, 1, a+l1+l2+l3);
                gsl_vector_set(connectionPointUp, 2, -(h2-s2));
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, a+l1+l2);
                gsl_vector_set(connectionPointDown, 2, s2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k4);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D2);
                DYTSI_Modelling::LateralSpring *spring = new DYTSI_Modelling::LateralSpring(
                        "Left lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorLeading->addLink(spring);
                DYTSI_Modelling::LateralDamper *damper = new DYTSI_Modelling::LateralDamper(
                        "Left lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorLeading->addLink(damper);
            };

            // Right lateral spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u1);
                gsl_vector_set(connectionPointUp, 1, -(a+l1+l2+l3));
                gsl_vector_set(connectionPointUp, 2, -(h2-s2));
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, -(a+l1+l2));
                gsl_vector_set(connectionPointDown, 2, s2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k4);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D2);
                DYTSI_Modelling::LateralSpring *spring = new DYTSI_Modelling::LateralSpring(
                        "Right lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorLeading->addLink(spring);
                DYTSI_Modelling::LateralDamper *damper = new DYTSI_Modelling::LateralDamper(
                        "Right lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorLeading->addLink(damper);
            };

            // Left Vertical Spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u1);
                gsl_vector_set(connectionPointUp, 1, s3);
                gsl_vector_set(connectionPointUp, 2, -v4);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, s3);
                gsl_vector_set(connectionPointDown, 2, v1+v2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k5);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D1);
                DYTSI_Modelling::VerticalSpring *spring = new DYTSI_Modelling::VerticalSpring(
                        "Left Vertical Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorLeading->addLink(spring);
                this->staticLoadedSecondarySuspensionSpringList.push_back(spring);
                DYTSI_Modelling::VerticalDamper *damper = new DYTSI_Modelling::VerticalDamper(
                        "Left Vertical Spring", connectionPointUp, connectionPointDown, dampingFunction);
                connectorLeading->addLink(damper);
            };

            // Right Vertical Spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u1);
                gsl_vector_set(connectionPointUp, 1, -s3);
                gsl_vector_set(connectionPointUp, 2, -v4);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, -s3);
                gsl_vector_set(connectionPointDown, 2, v1+v2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k5);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D1);
                DYTSI_Modelling::VerticalSpring *spring = new DYTSI_Modelling::VerticalSpring(
                        "Right Vertical Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorLeading->addLink(spring);
                this->staticLoadedSecondarySuspensionSpringList.push_back(spring);
                DYTSI_Modelling::VerticalDamper *damper = new DYTSI_Modelling::VerticalDamper(
                        "Right Vertical Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorLeading->addLink(damper);
            };

            // Left yaw spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u1 + s4);
                gsl_vector_set(connectionPointUp, 1, a+l1+l2+l3/2);
                gsl_vector_set(connectionPointUp, 2, -h2);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0.0);
                gsl_vector_set(connectionPointDown, 1, a+l1+l2+l3/2);
                gsl_vector_set(connectionPointDown, 2, 0.0);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D6);
                DYTSI_Modelling::LongitudinalDamper * damper = new DYTSI_Modelling::LongitudinalDamper(
                        "Left Yaw Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorLeading->addLink(damper);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k6);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Left Yaw Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorLeading->addLink(spring);
            };

            // Right yaw damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u1+s4);
                gsl_vector_set(connectionPointUp, 1, -(a+l1+l2+l3/2));
                gsl_vector_set(connectionPointUp, 2, -h2);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0.0);
                gsl_vector_set(connectionPointDown, 1, -(a+l1+l2+l3/2));
                gsl_vector_set(connectionPointDown, 2, 0.0);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D6);
                DYTSI_Modelling::LongitudinalDamper * damper = new DYTSI_Modelling::LongitudinalDamper(
                        "Right Yaw Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorLeading->addLink(damper);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k6);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Right Yaw Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorLeading->addLink(spring);
            };

            // End Connector Front Bogie Frame
            //////////////////////////////

            //////////////////////////////
            // Secondary Suspension Trailing bogie frame
            DYTSI_Modelling::BogieFrameCarBodyConnector *connectorTrailing =
                    new DYTSI_Modelling::BogieFrameCarBodyConnector(
                    "Secondary Suspension Trailing bogie frame", carBody,
                    trailingBogieFrame, DYTSI_Modelling::UP, DYTSI_Modelling::LT);

            // Left lateral spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u1);
                gsl_vector_set(connectionPointUp, 1, a+l1+l2+l3);
                gsl_vector_set(connectionPointUp, 2, -(h2-s2));
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, a+l1+l2);
                gsl_vector_set(connectionPointDown, 2, s2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k4);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D2);
                DYTSI_Modelling::LateralSpring *spring = new DYTSI_Modelling::LateralSpring(
                        "Left lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorTrailing->addLink(spring);
                DYTSI_Modelling::LateralDamper *damper = new DYTSI_Modelling::LateralDamper(
                        "Left lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorTrailing->addLink(damper);
            };

            // Right lateral spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u1);
                gsl_vector_set(connectionPointUp, 1, -(a+l1+l2+l3));
                gsl_vector_set(connectionPointUp, 2, -(h2-s2));
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, -(a+l1+l2));
                gsl_vector_set(connectionPointDown, 2, s2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k4);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D2);
                DYTSI_Modelling::LateralSpring *spring = new DYTSI_Modelling::LateralSpring(
                        "Right lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorTrailing->addLink(spring);
                DYTSI_Modelling::LateralDamper *damper = new DYTSI_Modelling::LateralDamper(
                        "Right lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorTrailing->addLink(damper);
            };

            // Left Vertical Spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u1);
                gsl_vector_set(connectionPointUp, 1, s3);
                gsl_vector_set(connectionPointUp, 2, -v4);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, s3);
                gsl_vector_set(connectionPointDown, 2, v1+v2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k5);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D1);
                DYTSI_Modelling::VerticalSpring *spring = new DYTSI_Modelling::VerticalSpring(
                        "Left Vertical Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorTrailing->addLink(spring);
                this->staticLoadedSecondarySuspensionSpringList.push_back(spring);
                DYTSI_Modelling::VerticalDamper *damper = new DYTSI_Modelling::VerticalDamper(
                        "Left Vertical Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorTrailing->addLink(damper);
            };

            // Right Vertical Spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u1);
                gsl_vector_set(connectionPointUp, 1, -s3);
                gsl_vector_set(connectionPointUp, 2, -v4);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0);
                gsl_vector_set(connectionPointDown, 1, -s3);
                gsl_vector_set(connectionPointDown, 2, v1+v2);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k5);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D1);
                DYTSI_Modelling::VerticalSpring *spring = new DYTSI_Modelling::VerticalSpring(
                        "Right Vertical Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorTrailing->addLink(spring);
                this->staticLoadedSecondarySuspensionSpringList.push_back(spring);
                DYTSI_Modelling::VerticalDamper *damper = new DYTSI_Modelling::VerticalDamper(
                        "Right Vertical Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorTrailing->addLink(damper);
            };

            // Left yaw spring and damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -(u1+s4));
                gsl_vector_set(connectionPointUp, 1, a+l1+l2+l3/2);
                gsl_vector_set(connectionPointUp, 2, -h2);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0.0);
                gsl_vector_set(connectionPointDown, 1, a+l1+l2+l3/2);
                gsl_vector_set(connectionPointDown, 2, 0.0);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D6);
                DYTSI_Modelling::LongitudinalDamper * damper = new DYTSI_Modelling::LongitudinalDamper(
                        "Left Yaw Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorTrailing->addLink(damper);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k6);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Left Yaw Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorTrailing->addLink(spring);
            };

            // Right yaw damper
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -(u1+s4));
                gsl_vector_set(connectionPointUp, 1, -(a+l1+l2+l3/2));
                gsl_vector_set(connectionPointUp, 2, -h2);
                gsl_vector* connectionPointDown = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointDown, 0, 0.0);
                gsl_vector_set(connectionPointDown, 1, -(a+l1+l2+l3/2));
                gsl_vector_set(connectionPointDown, 2, 0.0);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D6);
                DYTSI_Modelling::LongitudinalDamper * damper = new DYTSI_Modelling::LongitudinalDamper(
                        "Left Yaw Damper", connectionPointUp, connectionPointDown, dampingFunction);
                connectorTrailing->addLink(damper);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k6);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Right Yaw Spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                connectorTrailing->addLink(spring);
            };

            // End Connector Trailing Bogie Frame
            //////////////////////////////

        }; // End Bogie frames and upper connectors

        // Set up the wheelsets and connect them to the bogie frames
        {
            double mass = 1022.0;
            double Ix = 678.0;
            double Iy = 80.0;
            double Iz = 678.0;
            double a = 0.75;
            double mu = 0.15;
            double G = 2.1e11/(2*(1-0.27));
            std::vector<DYTSI_Modelling::RSGEOTable*> rsgeo_table;
            DYTSI_Input::RSGEOTableReader::read(rsgeo_table, this->getRSGEO_path(), '#');

            DYTSI_Modelling::WheelSetComponent *llwheelset;
            DYTSI_Modelling::WheelSetComponent *ltwheelset;
            DYTSI_Modelling::WheelSetComponent *tlwheelset;
            DYTSI_Modelling::WheelSetComponent *ttwheelset;
            DYTSI_Modelling::WheelSetBogieFrameConnector *llconnector;
            DYTSI_Modelling::WheelSetBogieFrameConnector *ltconnector;
            DYTSI_Modelling::WheelSetBogieFrameConnector *tlconnector;
            DYTSI_Modelling::WheelSetBogieFrameConnector *ttconnector;

            // Leading Bogie Frame - Leading wheelset
            {
                gsl_vector* centerOfGeometry = gsl_vector_calloc(3);
                gsl_vector_set(centerOfGeometry, 0, u1 + u2);
                gsl_vector_set(centerOfGeometry, 2, r0);
                gsl_vector* centerOfMass = gsl_vector_calloc(3);
                llwheelset = new DYTSI_Modelling::WheelSetComponent(
                        DYTSI_Modelling::LEADING, centerOfGeometry, centerOfMass,
                        mass,Ix,Iy,Iz,a,mu,G,
                        this->getExternalComponents(),
                        rsgeo_table,
                        "Leading Bogie - Leading Wheelset", this);
            };

            // Leading Bogie Frame - Trailing wheelset
            {
                gsl_vector* centerOfGeometry = gsl_vector_calloc(3);
                gsl_vector_set(centerOfGeometry, 0, u1 - u2);
                gsl_vector_set(centerOfGeometry, 2, r0);
                gsl_vector* centerOfMass = gsl_vector_calloc(3);
                ltwheelset = new DYTSI_Modelling::WheelSetComponent(
                        DYTSI_Modelling::TRAILING, centerOfGeometry, centerOfMass,
                        mass,Ix,Iy,Iz,a,mu,G,
                        this->getExternalComponents(),
                        rsgeo_table,
                        "Leading Bogie - Trailing Wheelset", this);
            };

            // Trailing Bogie Frame - Leading wheelset
            {
                gsl_vector* centerOfGeometry = gsl_vector_calloc(3);
                gsl_vector_set(centerOfGeometry, 0, - u1 + u2);
                gsl_vector_set(centerOfGeometry, 2, r0);
                gsl_vector* centerOfMass = gsl_vector_calloc(3);
                tlwheelset = new DYTSI_Modelling::WheelSetComponent(
                        DYTSI_Modelling::LEADING, centerOfGeometry, centerOfMass,
                        mass,Ix,Iy,Iz,a,mu,G,
                        this->getExternalComponents(),
                        rsgeo_table,
                        "Trailing Bogie - Leading Wheelset", this);
            };

            // Trailing Bogie Frame - Trailing wheelset
            {
                gsl_vector* centerOfGeometry = gsl_vector_calloc(3);
                gsl_vector_set(centerOfGeometry, 0, -u1 - u2);
                gsl_vector_set(centerOfGeometry, 2, r0);
                gsl_vector* centerOfMass = gsl_vector_calloc(3);
                ttwheelset = new DYTSI_Modelling::WheelSetComponent(
                        DYTSI_Modelling::TRAILING, centerOfGeometry, centerOfMass,
                        mass,Ix,Iy,Iz,a,mu,G,
                        this->getExternalComponents(),
                        rsgeo_table,
                        "Trailing Bogie - Trailing Wheelset", this);
            };

            ////////////////////////////////
            // Primary Suspension: Leading Bogie Frame - Leading Wheelset
            llconnector = new DYTSI_Modelling::WheelSetBogieFrameConnector(
                    "Primary Suspension: Leading Bogie Frame - Leading Wheelset",
                    leadingBogieFrame, llwheelset,
                    DYTSI_Modelling::UP, DYTSI_Modelling::LL);

            // Left lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, a+l1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Left lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                llconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                llconnector->addLink(damper);
            };

            // Right lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, -(a+l1));
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Right lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                llconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                llconnector->addLink(damper);
            };

            // Left vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Left vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                llconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                llconnector->addLink(damper);
            };

            // Right vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Right vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                llconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                llconnector->addLink(damper);
            };

            // Left longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, x1);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Left longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                llconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                llconnector->addLink(damper);
            };

            // Right longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, x1);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Right longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                llconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                llconnector->addLink(damper);
            };
            // End Setup connector: Leading Bogie Frame - Leading Wheelset
            ////////////////////////////////

            ////////////////////////////////
            // Primary Suspension: Leading Bogie Frame - Trailing Wheelset
            ltconnector = new DYTSI_Modelling::WheelSetBogieFrameConnector(
                    "Primary Suspension: Leading Bogie Frame - Trailing Wheelset",
                    leadingBogieFrame, ltwheelset,
                    DYTSI_Modelling::UP, DYTSI_Modelling::LT);

            // Left lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, a+l1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Left lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ltconnector->addLink(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                ltconnector->addLink(damper);
            };

            // Right lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, -(a+l1));
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Right lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ltconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                ltconnector->addLink(damper);
            };

            // Left vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Left vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ltconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                ltconnector->addLink(damper);
            };

            // Right vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Right vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ltconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                ltconnector->addLink(damper);
            };

            // Left longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -x1);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Left longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ltconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                ltconnector->addLink(damper);
            };

            // Right longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -x1);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Right longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ltconnector->addLink(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                ltconnector->addLink(damper);
            };
            // End Setup connector: Leading Bogie Frame - Trailing Wheelset
            ////////////////////////////////

            ////////////////////////////////
            // Primary Suspension: Trailing Bogie Frame - Leading Wheelset
            tlconnector = new DYTSI_Modelling::WheelSetBogieFrameConnector(
                    "Primary Suspension: Trailing Bogie Frame - Leading Wheelset",
                    trailingBogieFrame, tlwheelset,
                    DYTSI_Modelling::UP, DYTSI_Modelling::LL);

            // Left lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, a+l1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Left lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                tlconnector->addLink(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                tlconnector->addLink(damper);
            };

            // Right lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, -(a+l1));
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Right lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                tlconnector->addLink(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                tlconnector->addLink(damper);
            };

            // Left vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Left vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                tlconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                tlconnector->addLink(damper);
            };

            // Right vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, u2);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Right vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                tlconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                tlconnector->addLink(damper);
            };

            // Left longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, x1);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Left longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                tlconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                tlconnector->addLink(damper);
            };

            // Right longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, x1);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Right longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                tlconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                tlconnector->addLink(damper);
            };
            // End Setup connector: Trailing Bogie Frame - Leading Wheelset
            ////////////////////////////////

            ////////////////////////////////
            // Primary Suspension: Trailing Bogie Frame - Trailing Wheelset
            ttconnector = new DYTSI_Modelling::WheelSetBogieFrameConnector(
                    "Primary Suspension: Trailing Bogie Frame - Trailing Wheelset",
                    trailingBogieFrame, ttwheelset,
                    DYTSI_Modelling::UP, DYTSI_Modelling::LT);

            // Left lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, a+l1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Left lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ttconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                ttconnector->addLink(damper);
            };

            // Right lateral spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, -(a+l1));
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -a);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k1);
                DYTSI_Modelling::LateralSpring* spring = new DYTSI_Modelling::LateralSpring(
                        "Right lateral spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ttconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D5);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right lateral damper", connectionPointUp, connectionPointDown, dampingFunction);
                ttconnector->addLink(damper);
            };

            // Left vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Left vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ttconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                ttconnector->addLink(damper);
            };

            // Right vertical spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -u2);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, v1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k3);
                DYTSI_Modelling::VerticalSpring* spring = new DYTSI_Modelling::VerticalSpring(
                        "Right vertical spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ttconnector->addLink(spring);
                this->staticLoadedPrimarySuspensionSpringList.push_back(spring);

                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D4);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right vertical damper", connectionPointUp, connectionPointDown, dampingFunction);
                ttconnector->addLink(damper);
            };

            // Left longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -x1);
                gsl_vector_set(connectionPointUp, 1, s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Left longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ttconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Left longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                ttconnector->addLink(damper);
            };

            // Right longitudinal spring
            {
                gsl_vector* connectionPointUp = gsl_vector_alloc(3);
                gsl_vector_set(connectionPointUp, 0, -x1);
                gsl_vector_set(connectionPointUp, 1, -s1);
                gsl_vector_set(connectionPointUp, 2, -h1);
                gsl_vector* connectionPointDown = gsl_vector_calloc(3);
                gsl_vector_set(connectionPointDown, 1, -s1);
                DYTSI_Modelling::LinearFunction* stiffnessFunction = new DYTSI_Modelling::LinearFunction(k2);
                DYTSI_Modelling::LongitudinalSpring* spring = new DYTSI_Modelling::LongitudinalSpring(
                        "Right longitudinal spring", connectionPointUp, connectionPointDown, stiffnessFunction);
                ttconnector->addLink(spring);
                DYTSI_Modelling::LinearFunction* dampingFunction = new DYTSI_Modelling::LinearFunction(D3);
                DYTSI_Modelling::VectorDamper* damper = new DYTSI_Modelling::VectorDamper(
                        "Right longitudinal damper", connectionPointUp, connectionPointDown, dampingFunction);
                ttconnector->addLink(damper);
            };
            // End Setup connector: Leading Bogie Frame - Leading Wheelset
            ////////////////////////////////
        };

        return error;
    }

    double CooperriderOrthogonalModel::getD1() const {
        return D1;
    }

    void CooperriderOrthogonalModel::setD1(double D1) {
        this->D1 = D1;
    }

    double CooperriderOrthogonalModel::getD2() const {
        return D2;
    }

    void CooperriderOrthogonalModel::setD2(double D2) {
        this->D2 = D2;
    }

    double CooperriderOrthogonalModel::getD3() const {
        return D3;
    }

    void CooperriderOrthogonalModel::setD3(double D3) {
        this->D3 = D3;
    }

    double CooperriderOrthogonalModel::getD4() const {
        return D4;
    }

    void CooperriderOrthogonalModel::setD4(double D4) {
        this->D4 = D4;
    }

    double CooperriderOrthogonalModel::getD5() const {
        return D5;
    }

    void CooperriderOrthogonalModel::setD5(double D5) {
        this->D5 = D5;
    }

    double CooperriderOrthogonalModel::getD6() const {
        return D6;
    }

    void CooperriderOrthogonalModel::setD6(double D6) {
        this->D6 = D6;
    }

    double CooperriderOrthogonalModel::getK1() const {
        return k1;
    }

    void CooperriderOrthogonalModel::setK1(double k1) {
        this->k1 = k1;
    }

    double CooperriderOrthogonalModel::getK2() const {
        return k2;
    }

    void CooperriderOrthogonalModel::setK2(double k2) {
        this->k2 = k2;
    }

    double CooperriderOrthogonalModel::getK3() const {
        return k3;
    }

    void CooperriderOrthogonalModel::setK3(double k3) {
        this->k3 = k3;
    }

    double CooperriderOrthogonalModel::getK4() const {
        return k4;
    }

    void CooperriderOrthogonalModel::setK4(double k4) {
        this->k4 = k4;
    }

    double CooperriderOrthogonalModel::getK5() const {
        return k5;
    }

    void CooperriderOrthogonalModel::setK5(double k5) {
        this->k5 = k5;
    }

    double CooperriderOrthogonalModel::getK6() const {
        return k6;
    }

    void CooperriderOrthogonalModel::setK6(double k6) {
        this->k6 = k6;
    }

    double CooperriderOrthogonalModel::getH1() const {
        return h1;
    }

    void CooperriderOrthogonalModel::setH1(double h1) {
        this->h1 = h1;
    }

    double CooperriderOrthogonalModel::getH2() const {
        return h2;
    }

    void CooperriderOrthogonalModel::setH2(double h2) {
        this->h2 = h2;
    }

    double CooperriderOrthogonalModel::getR0() const {
        return r0;
    }

    void CooperriderOrthogonalModel::setR0(double r0) {
        this->r0 = r0;
    }

    double CooperriderOrthogonalModel::getA() const {
        return a;
    }

    void CooperriderOrthogonalModel::setA(double a) {
        this->a = a;
    }

    void CooperriderOrthogonalModel::setL1(double val){
        this->l1 = val;
    }

    void CooperriderOrthogonalModel::setL2(double val){
        this->l2 = val;
    }

    void CooperriderOrthogonalModel::setL3(double val){
        this->l3 = val;
    }

    void CooperriderOrthogonalModel::setV1(double val){
        this->v1 = val;
    }

    void CooperriderOrthogonalModel::setV2(double val){
        this->v2 = val;
    }

    void CooperriderOrthogonalModel::setV3(double val){
        this->v3 = val;
    }

    void CooperriderOrthogonalModel::setV4(double val){
        this->v4 = val;
    }

    void CooperriderOrthogonalModel::setU1(double val){
        this->u1 = val;
    }

    void CooperriderOrthogonalModel::setU2(double val){
        this->u2 = val;
    }

    void CooperriderOrthogonalModel::setX1(double val){
        this->x1 = val;
    }

    void CooperriderOrthogonalModel::setS1(double val){
        this->s1 = val;
    }

    void CooperriderOrthogonalModel::setS2(double val){
        this->s2 = val;
    }

    void CooperriderOrthogonalModel::setS3(double val){
        this->s3 = val;
    }

    void CooperriderOrthogonalModel::setS4(double val){
        this->s4 = val;
    }

    double CooperriderOrthogonalModel::getL1() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getL2() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getL3() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getV1() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getV2() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getV3() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getV4() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getU1() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getU2() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getX1() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getS1() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getS2() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getS3() const{
        return this->l1;
    }

    double CooperriderOrthogonalModel::getS4() const{
        return this->l1;
    }

    XERCES_CPP_NAMESPACE::DOMNode* CooperriderOrthogonalModel::getRSGEO_path() const {
        return this->pathRSGEO;
    }

    void CooperriderOrthogonalModel::setRSGEO_path(XERCES_CPP_NAMESPACE::DOMNode* RSGEO_Path){
        this->pathRSGEO = RSGEO_Path;
    }
}


