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
 * File:   Input.cpp
 * Author: bigo
 * 
 * Created on March 11, 2011, 1:29 PM
 */

#include <vector>
#include <sstream>
#include <fstream>

#include "Input.h"

#include "XMLaux.h"
#include "../PrintingHandler.h"

#include "../../Models/DYTSI_Models.h"
#include "../../Solver/DYTSI_Solvers.h"
#include "InputHandler.h"
#include "../Output/OutputHandler.h"

namespace DYTSI_Input{
  Input::Input( DYTSI_Input::InputHandler* inputHandler, XERCES_CPP_NAMESPACE::DOMElement* node, int& error) {
    this->inputHandler = inputHandler;

    XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = node->getAttributes();
    XERCES_CPP_NAMESPACE::DOMAttr* nameAttribute =
      (XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                                                                XERCES_CPP_NAMESPACE::XMLString::transcode("TestName"));
    this->testName = XERCES_CPP_NAMESPACE::XMLString::transcode(nameAttribute->getValue());

    std::stringstream* out = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *out << "\tTest Name: " << this->testName << std::endl;
    PrintingHandler::printOut(out,PrintingHandler::STDOUT);
    delete out;

    // Create the output folder for the test
    std::stringstream* outFolder = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *outFolder << this->inputHandler->getOutputFolder() << "/" << this->testName;
    error = this->inputHandler->createOutputFolder(outFolder->str().c_str());
    if (error != 0) return;

    // Store the test information in the folder
    std::stringstream* xmlInput = new std::stringstream(std::stringstream::in|std::stringstream::out);
    XMLaux::serializeDOM(node,xmlInput);
    std::stringstream* xmlInputPath = new std::stringstream(std::stringstream::in|std::stringstream::out);
    *xmlInputPath << outFolder->str() << "/input.xml";
    std::ofstream xmlInputFile;
    xmlInputFile.open(xmlInputPath->str().c_str());
    xmlInputFile << xmlInput->str();
    xmlInputFile.close();

    // Setting up the model
    error = 0;
    XERCES_CPP_NAMESPACE::DOMNodeList* modelsList = node->getElementsByTagName(
                                                                               XERCES_CPP_NAMESPACE::XMLString::transcode("Model"));
    if (modelsList->getLength() == 1){
      XERCES_CPP_NAMESPACE::DOMElement* model =
        (XERCES_CPP_NAMESPACE::DOMElement*)modelsList->item(0);
      XERCES_CPP_NAMESPACE::DOMNodeList* cooperriderList = 
        model->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("CooperriderModel"));
      XERCES_CPP_NAMESPACE::DOMNodeList* cooperriderOrthogonalList = 
        model->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("CooperriderOrthogonalModel"));
      XERCES_CPP_NAMESPACE::DOMNodeList* cooperriderOrthogonalFixedList = 
        model->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("CooperriderOrthogonalFixedModel"));
      XERCES_CPP_NAMESPACE::DOMNodeList* cooperriderOrthogonalAsymmetricList = 
        model->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("CooperriderOrthogonalAsymmetricModel"));
      XERCES_CPP_NAMESPACE::DOMNodeList* cooperriderOrthoAsymFixedList = 
        model->getElementsByTagName(XERCES_CPP_NAMESPACE::XMLString::transcode("CooperriderOrthoAsymFixedModel"));

      if (cooperriderList->getLength() == 1){
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tCooperrider Model. " << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        XERCES_CPP_NAMESPACE::DOMElement* cooperrider =
          (XERCES_CPP_NAMESPACE::DOMElement*)cooperriderList->item(0);
        this->model = new DYTSI_Models::CooperriderModel();
        this->model->setup(cooperrider);
      } else if (cooperriderOrthogonalList->getLength() == 1) {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tCooperrider Orthogonal Model. " << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        XERCES_CPP_NAMESPACE::DOMElement* cooperrider =
          (XERCES_CPP_NAMESPACE::DOMElement*)cooperriderOrthogonalList->item(0);
        this->model = new DYTSI_Models::CooperriderOrthogonalModel();
        this->model->setup(cooperrider);
      } else if (cooperriderOrthogonalFixedList->getLength() == 1) {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tCooperrider Orthogonal Fixed Model. " << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        XERCES_CPP_NAMESPACE::DOMElement* cooperrider =
          (XERCES_CPP_NAMESPACE::DOMElement*)cooperriderOrthogonalFixedList->item(0);
        this->model = new DYTSI_Models::CooperriderOrthogonalFixedModel();
        this->model->setup(cooperrider);
      } else if (cooperriderOrthogonalAsymmetricList->getLength() == 1) {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tCooperrider Orthogonal Asymmetric Model. " << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        XERCES_CPP_NAMESPACE::DOMElement* cooperrider =
          (XERCES_CPP_NAMESPACE::DOMElement*)cooperriderOrthogonalAsymmetricList->item(0);
        this->model = new DYTSI_Models::CooperriderOrthogonalAsymmetricModel();
        this->model->setup(cooperrider);
      } else if (cooperriderOrthoAsymFixedList->getLength() == 1) {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tCooperrider Orthogonal Asymmetric Fixed Model. " << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;

        XERCES_CPP_NAMESPACE::DOMElement* cooperrider =
          (XERCES_CPP_NAMESPACE::DOMElement*)cooperriderOrthoAsymFixedList->item(0);
        this->model = new DYTSI_Models::CooperriderOrthoAsymFixedModel();
        this->model->setup(cooperrider);
      } else {
        std::stringstream out;
        out << "Error in parsing the input file: " << std::endl;
        out << "\t " << this->testName << ": The model in this test is not defined properly." << std::endl;
        PrintingHandler::printOut(&out, PrintingHandler::STDERR);
        error = 1;
        return;
      }
    } else {
      std::stringstream out;
      out << "Error in parsing the input file: " << std::endl;
      out << "\t " << this->testName << ": The model in this test is not defined properly." << std::endl;
      PrintingHandler::printOut(&out, PrintingHandler::STDERR);
      error = 1;
      return;
    }

    // Setting up the transient curve
    this->transientCurve = new DYTSI_Input::TransientCurve();
    XERCES_CPP_NAMESPACE::DOMNodeList* transientRadiusList = node->getElementsByTagName(
                                                                                        XERCES_CPP_NAMESPACE::XMLString::transcode("TransientRadius"));
    if (transientRadiusList->getLength() == 1){
      XERCES_CPP_NAMESPACE::DOMElement* transientRadiusEl =
        (XERCES_CPP_NAMESPACE::DOMElement*)transientRadiusList->item(0);
      // Get the type of transient radius
      XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = transientRadiusEl->getAttributes();
      XERCES_CPP_NAMESPACE::DOMAttr* transientAttribute =(XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                                                                                                                   XERCES_CPP_NAMESPACE::XMLString::transcode("type"));
      char* transientType = XERCES_CPP_NAMESPACE::XMLString::transcode(transientAttribute->getValue());
      if (strcmp(transientType,"lin") == 0){
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tLinear Transient Radius Construction" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->transientCurve->setTransientRadius(transientRadiusEl, error);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tThe transient " << transientType << " doesn't exists." << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error = 1;
        return;
      }
    }
    XERCES_CPP_NAMESPACE::DOMNodeList* transientCantList = node->getElementsByTagName(
                                                                                      XERCES_CPP_NAMESPACE::XMLString::transcode("TransientPhi_se"));
    if (transientCantList->getLength() == 1){
      XERCES_CPP_NAMESPACE::DOMElement* transientCantEl =
        (XERCES_CPP_NAMESPACE::DOMElement*)transientCantList->item(0);
      // Get the type of transient Cant
      XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = transientCantEl->getAttributes();
      XERCES_CPP_NAMESPACE::DOMAttr* transientAttribute =(XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                                                                                                                   XERCES_CPP_NAMESPACE::XMLString::transcode("type"));
      char* transientType = XERCES_CPP_NAMESPACE::XMLString::transcode(transientAttribute->getValue());
      if (strcmp(transientType,"lin") == 0){
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tLinear Transient Cant Construction" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->transientCurve->setTransientCant(transientCantEl, error);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tThe transient " << transientType << " doesn't exists." << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error = 1;
        return;
      }
    }

    // Setting up the Solver
    XERCES_CPP_NAMESPACE::DOMNodeList* solverList = node->getElementsByTagName(
                                                                               XERCES_CPP_NAMESPACE::XMLString::transcode("Solver"));
    if (solverList->getLength() == 1){
      XERCES_CPP_NAMESPACE::DOMElement* solverEl =
        (XERCES_CPP_NAMESPACE::DOMElement*)solverList->item(0);
      // Get the name of the method
      XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = solverEl->getAttributes();
      XERCES_CPP_NAMESPACE::DOMAttr* methodNameAttribute =(XERCES_CPP_NAMESPACE::DOMAttr*)pAttributes->getNamedItem(
                                                                                                                    XERCES_CPP_NAMESPACE::XMLString::transcode("Name"));
      char* methodName = XERCES_CPP_NAMESPACE::XMLString::transcode(methodNameAttribute->getValue());
      if (strcmp(methodName,"bsimp") == 0){
        // Solver construction
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Construction: \"Implicit Bulirsch-Stoer\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->solver = new DYTSI_Solver::BulirschStoerSolver(this->model, solverEl, error);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }

        // Solver setup
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Setup: \"Implicit Bulirsch-Stoer\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error += this->solver->setup();
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else if (strcmp(methodName,"bdf") == 0){
        // Solver construction
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Construction: \"BDF Method\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->solver = new DYTSI_Solver::BDFSolver(this->model, solverEl, error);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }

        // Solver setup
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Setup: \"BDF method\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error += this->solver->setup();
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else if (strcmp(methodName,"rk4imp") == 0){
        // Solver construction
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Construction: \"Implicit Runge-Kutta 4th\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->solver = new DYTSI_Solver::RK4ImplicitAtGaussianPoints(this->model, solverEl, error);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }

        // Solver setup
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Setup: \"Implicit Runge-Kutta 4th\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error += this->solver->setup();
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else if (strcmp(methodName,"rkf34") == 0){
        // Solver construction
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Construction: \"RKF34\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->solver = new DYTSI_Solver::SDIRKSolver(this->model, solverEl, error, RKF34);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }

        // Solver setup
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Setup: \"RKF34\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error += this->solver->setup();
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else if (strcmp(methodName,"sdirkNT1") == 0){
        // Solver construction
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Construction: \"SDIRK\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->solver = new DYTSI_Solver::SDIRKSolver(this->model, solverEl, error, NT1);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }

        // Solver setup
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Setup: \"SDIRK\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error += this->solver->setup();
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else if (strcmp(methodName,"sdirkJKT") == 0){
        // Solver construction
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Construction: \"SDIRK\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        this->solver = new DYTSI_Solver::SDIRKSolver(this->model, solverEl, error, JKT);
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }

        // Solver setup
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tSolver Setup: \"SDIRK\"" << std::flush;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error += this->solver->setup();
        if (error){
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[FAILED]" << std::endl;
          *out << "Error Code: " << error << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
          error = 1;
          return;
        } else {
          out = new std::stringstream(std::stringstream::in|std::stringstream::out);
          *out << "\t[DONE]" << std::endl;
          PrintingHandler::printOut(out,PrintingHandler::STDOUT);
          delete out;
        }
      } else {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t\tThe solver " << methodName << " doesn't exists." << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error = 1;
        return;
      }
    } else {
      out = new std::stringstream(std::stringstream::in|std::stringstream::out);
      *out << "\t\tThe solver is not defined properly." << std::endl;
      PrintingHandler::printOut(out,PrintingHandler::STDOUT);
      delete out;
      error = 1;
      return;
    }

    // Setting up the OutputHandler
    XERCES_CPP_NAMESPACE::DOMNodeList* outputList = node->getElementsByTagName(
                                                                               XERCES_CPP_NAMESPACE::XMLString::transcode("Output"));
    if (outputList->getLength() == 1){
      XERCES_CPP_NAMESPACE::DOMElement* outputEl = (XERCES_CPP_NAMESPACE::DOMElement*)outputList->item(0);

      // Output Handler construction
      out = new std::stringstream(std::stringstream::in|std::stringstream::out);
      *out << "\t\tOutput Handler Construction" << std::flush;
      PrintingHandler::printOut(out,PrintingHandler::STDOUT);
      delete out;

      this->outputHandler = new DYTSI_Output::OutputHandler(this, outputEl, error);

      if (error){
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t[FAILED]" << std::endl;
        *out << "Error Code: " << error << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
        error = 1;
        return;
      } else {
        out = new std::stringstream(std::stringstream::in|std::stringstream::out);
        *out << "\t[DONE]" << std::endl;
        PrintingHandler::printOut(out,PrintingHandler::STDOUT);
        delete out;
      }

    } else {
      out = new std::stringstream(std::stringstream::in|std::stringstream::out);
      *out << "\t\tThe Output property is not defined properly." << std::endl;
      PrintingHandler::printOut(out,PrintingHandler::STDOUT);
      delete out;
      error = 1;
      return;
    }
  }

  Input::~Input() {
  }

  DYTSI_Modelling::GeneralModel* Input::getModel(){
    return this->model;
  }

  DYTSI_Input::InputHandler* Input::getInputHandler(){
    return this->inputHandler;
  }

  DYTSI_Solver::Solver* Input::getSolver(){
    return this->solver;
  }

  DYTSI_Output::OutputHandler* Input::getOutputHandler(){
    return this->outputHandler;
  }

  int Input::coverTransientCurve(int index, DYTSI_Solver::Solution &sol,
                                 double speedSim, double radiusSim, double Phi_seSim){
    int errCode;
    std::stringstream* out;
    double radiusTime;
    double cantTime;

    // Starting the Transient analysis
    out = new std::stringstream(std::stringstream::in | std::stringstream::out);
    *out << std::endl << "#####################################" << std::endl;
    *out << "\t Transient Analysis of the Transient Curve: " << this->getTestName() << std::endl;
    PrintingHandler::printOut(out, PrintingHandler::STDOUT);
    delete out;

    // Set the initial starting components
    this->getModel()->getExternalComponents()->set_v(speedSim);
    if (!this->transientCurve->isSettedRadius()){
      this->getModel()->getExternalComponents()->set_R(radiusSim);
      radiusTime = 0.0;
    } else {
      this->getModel()->getExternalComponents()->set_R(this->transientCurve->getRadiusStart());
      radiusTime = this->transientCurve->getRadiusTime();
    }
    if (!this->transientCurve->isSettedCant()){
      this->getModel()->getExternalComponents()->set_Phi_se(Phi_seSim);
      cantTime = 0.0;
    } else {
      this->getModel()->getExternalComponents()->set_Phi_se(this->transientCurve->getCantStart());
      cantTime = this->transientCurve->getCantTime();
    }

    // Set the time span for the simulation
    DYTSI_Input::Range *tSpan = new DYTSI_Input::Range();
    tSpan->setStartValue(0.0);
    tSpan->setEndValue( (radiusTime > cantTime) ? radiusTime : cantTime );

    // Starting values
    double* yInit = new double[this->getModel()->get_N_DOF()];
    for (int i = 0; i < this->getModel()->get_N_DOF(); i++) yInit[i] = this->y0[i];

    // Solve the system and get the timing
    errCode = this->getSolver()->solveTransient(index, yInit, tSpan, sol, this->transientCurve);

    // Ending the transient analysis
    out = new std::stringstream(std::stringstream::in | std::stringstream::out);
    *out << std::endl << "\t Transient Analysis of the Transient Curve: " << this->getTestName() << "\t[DONE]"<< std::endl;
    *out << "#####################################" << std::endl;
    PrintingHandler::printOut(out, PrintingHandler::STDOUT);
    delete out;

    return errCode;

  }
}
