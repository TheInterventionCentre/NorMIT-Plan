
/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationSeedNode.h

  Copyright (c) 2017, The Intervention Centre, Oslo University Hospital

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =========================================================================*/

#ifndef __vtkMRMLVesselSegmentationSeedNode_h
#define __vtkMRMLVesselSegmentationSeedNode_h


// This module includes.
#include "vtkSlicerResectionPlanningModuleMRMLExport.h"

// MRML includes
#include <vtkNew.h>
#include <vtkMRMLNode.h>

/**
 * \ingroup VesselSegmentation
 *
 * \brief This class represents the seeds (3D points) used for vessel segmentation and splitting.
 * It inherits from vtkMRMLNode.
 */
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRML_EXPORT
vtkMRMLVesselSegmentationSeedNode : public vtkMRMLNode
{
public:

  /**
   * Standard vtk object instantiation method.
   *
   * @return a pointer to a newly created vtkMRMLVesselSegmentationSeedNode.
   */
  static vtkMRMLVesselSegmentationSeedNode *New();

  vtkTypeMacro(vtkMRMLVesselSegmentationSeedNode,vtkMRMLNode);

  /**
   * Standard print object information method.
   *
   * @param os output stream to print the information to.
   * @param indent indent value.
   */
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Standard MRML method to create the node instance.
   *
   * @return a pointer to the new created vtkMRMLNode.
   */
  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  /**
   * Get the tag name of the node.
   *
   * @return string with the tag name of the node.
   */
  virtual const char* GetNodeTagName() VTK_OVERRIDE {return "VesselSegmentationSeed";};

  /**
   * Get the icon associated to the node.
   *
   * @return string pointing to the resource where the icon is located.
   */
  virtual const char* GetIcon() {return "";}

  /// Get/Set Current seed mode.
  vtkGetMacro(CurrentSeedMode, int);
  void SetCurrentSeedMode(int mode);

  enum
    {
      BaseMode = 0x1,
      PlaceSeedSeg = 0x2,
      PlaceSeedSplit = 0x3,
    };

  /**
   * Set seed1
   *
   * @param value of seed1 double x.
   * @param value of seed1 double y.
   * @param value of seed1 double z.
   */
  void SetSeed1(double x, double y, double z);

  /**
   * Set seed2
   *
   * @param value of seed2 double x.
   * @param value of seed2 double y.
   * @param value of seed2 double z.
   */
  void SetSeed2(double x, double y, double z);

  /**
   * Get seed1
   *
   * @return value of seed1 double* x,y,z.
   */
  double* GetSeed1();

  /**
   * Get seed2
   *
   * @return value of seed2 double* x,y,z.
   */
  double* GetSeed2();
  
  /**
   * Get seed2 state (set or not set)
   *
   * @return value of seed1Set.
   */
  bool GetIsSeed1Set();
  
  /**
   * Get seed2 state (set or not set)
   *
   * @return value of seed2Set.
   */
  bool GetIsSeed2Set();

protected:
  vtkMRMLVesselSegmentationSeedNode();
  ~vtkMRMLVesselSegmentationSeedNode();

  vtkMRMLVesselSegmentationSeedNode(const vtkMRMLVesselSegmentationSeedNode&);
  void operator=(const vtkMRMLVesselSegmentationSeedNode&);

  double Seed1 [3];
  bool Seed1Set;
  double Seed2 [3];
  bool Seed2Set;

  int CurrentSeedMode;

};


#endif /* RESECTIONPLANNING_MRML_VTKMRMLVESSELSEGMENTATIONSEEDNODE_H_ */
