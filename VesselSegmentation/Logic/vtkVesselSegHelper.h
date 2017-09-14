#ifndef __vtkVesselSegHelper_h
#define __vtkVesselSegHelper_h

#include <vtkObject.h>
#include <vtkSmartPointer.h>

#include <itkImage.h>
#include <itkPoint.h>
#include <itkIndex.h>
#include "itkSeedVesselSegmentationImageFilter.h"

class vtkMRMLScene;
class vtkMRMLScalarVolumeNode;
class vtkImageData;
class vtkMatrix4x4;


/**
 *  Class that contains helper methods to get / prepare the data needed for calling Rahul's algorithm.
 *  This includes conversion between itk and vtk images.
 */
class vtkVesselSegHelper: public vtkObject
{
    
public:
    
    static vtkVesselSegHelper *New();
    vtkTypeMacro(vtkVesselSegHelper, vtkObject);

    typedef itk::Index<3> Index3D;
    typedef itk::Vector<double,3> Vector3D;

    typedef float pixelType;
    typedef itk::Image<short, 3> LabelMapType;
    typedef itk::Image<pixelType, 3> SeedImageType;

    static void GetActiveNode(vtkMRMLScene* myMRMLScene);
    
    void PrintSelf(ostream &os, vtkIndent indent);

    static bool ConvertRAStoLPS(double *inPoint, double *outPoint);
    
    static SeedImageType::Pointer ConvertVolumeNodeToItkImage(vtkMRMLScalarVolumeNode *inVolumeNode,
                           bool applyRasToWorld=true,
                           bool applyRasToLps=true);
    static SeedImageType::Pointer ConvertVtkImageDataToItkImage(vtkImageData *inImageData,
                             vtkMatrix4x4 *inToRasMatrix=NULL,
                             vtkMatrix4x4 *inToWorldMatrix=NULL,
                             vtkMatrix4x4 *inRasToLpsMatrix=NULL);

    static vtkSmartPointer<vtkImageData> ConvertItkImageToVtkImageData(SeedImageType::Pointer itkImage);
    static vtkSmartPointer<vtkImageData> ConvertItkImageToVtkImageData(itk::Image<unsigned int, 3>::Pointer itkImage);
    static vtkSmartPointer<vtkMRMLScalarVolumeNode> ConvertItkImageToVolumeNode(SeedImageType::Pointer itkImage, bool applyRasToLps=true);
    static vtkSmartPointer<vtkMRMLScalarVolumeNode> ConvertVtkImageDataToVolumeNode(vtkImageData *inImageData, SeedImageType::Pointer itkImage, bool applyLpsToRas);

protected:

    vtkVesselSegHelper();
    ~vtkVesselSegHelper();
    
private:
    
    
};

#endif
