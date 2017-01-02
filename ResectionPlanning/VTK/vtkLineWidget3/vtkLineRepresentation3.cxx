/*=========================================================================
  Program:   Visualization Toolkit
  Module:    vtkLineRepresentation3.cxx
  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.
=========================================================================*/
#include "vtkLineRepresentation3.h"

#include <vtkPointHandleRepresentation3D.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkCallbackCommand.h>
#include <vtkBox.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorObserver.h>
#include <vtkMath.h>
#include <vtkLine.h>
#include <vtkWindow.h>
#include <vtkObjectFactory.h>
#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkCellPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>

vtkStandardNewMacro(vtkLineRepresentation3);

vtkCxxSetObjectMacro(vtkLineRepresentation3,HandleRepresentation,vtkPointHandleRepresentation3D);

//----------------------------------------------------------------------------
vtkLineRepresentation3::vtkLineRepresentation3()
{
  // Handle size is in pixels for this widget
  this->HandleSize = 5.0;

  // By default, use one of these handles
  this->HandleRepresentation = vtkPointHandleRepresentation3D::New();
  this->HandleRepresentation->AllOff();
  this->HandleRepresentation->SetHotSpotSize(1.0);
  this->HandleRepresentation->SetPlaceFactor(1.0);
  this->HandleRepresentation->TranslationModeOn();
  this->Point1Representation = NULL;
  this->LineHandleRepresentation = NULL;
  this->InstantiateHandleRepresentation();

  // Miscellaneous parameters
  this->Tolerance = 5;
  this->Placed = 0;

  // Represent the line
  this->LineSource = vtkLineSource::New();
  this->LineSource->SetResolution(5);
  this->LineMapper = vtkPolyDataMapper::New();
  this->LineMapper->SetInputConnection(
    this->LineSource->GetOutputPort());
  this->LineActor = vtkActor::New();
  this->LineActor->SetMapper(this->LineMapper);

  // Create the handles
  this->Handle = vtkActor::New();
  this->HandleMapper = vtkPolyDataMapper::New();
  this->HandleGeometry = vtkPolyDataAlgorithm::New();

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetThetaResolution(16);
  sphere->SetPhiResolution(8);

  this->HandleGeometry = sphere;
  this->HandleMapper = vtkPolyDataMapper::New();
  this->HandleMapper->SetInputConnection(this->HandleGeometry->GetOutputPort());
  this->Handle = vtkActor::New();
  this->Handle->SetMapper(this->HandleMapper);

  // Set up the initial properties
  this->CreateDefaultProperties();

  // Pass the initial properties to the actors.
  this->Handle->SetProperty(this->EndPointProperty);
  this->Point1Representation->SetProperty(this->EndPointProperty);
  this->LineHandleRepresentation->SetProperty(this->EndPointProperty);
  this->LineActor->SetProperty(this->LineProperty);

  // Define the point coordinates
  double bounds[6];
  bounds[0] = -0.5;
  bounds[1] = 0.5;
  bounds[2] = -0.5;
  bounds[3] = 0.5;
  bounds[4] = -0.5;
  bounds[5] = 0.5;
  this->PlaceFactor = 1.0; //overload parent's value

  // The distance text annotation
  this->DistanceAnnotationVisibility = 0;
  this->Distance = 0.0;
  this->DistanceAnnotationFormat = new char[8];
  sprintf(this->DistanceAnnotationFormat,"%s","%-#6.3g");
  this->TextInput = vtkVectorText::New();
  this->TextInput->SetText( "0" );
  this->TextMapper = vtkPolyDataMapper::New();
  this->TextMapper->SetInputConnection(this->TextInput->GetOutputPort());
  this->TextActor = vtkFollower::New();
  this->TextActor->SetMapper(this->TextMapper);
  this->TextActor->GetProperty()->SetColor( 1.0, 0.1, 0.0 );

  // This needs to be initialized before PlaceWidget is called.
  this->InitializedDisplayPosition = 0;

  this->ClampToBounds = 0;

  // The bounding box
  this->BoundingBox = vtkBox::New();

  this->LinePicker = vtkCellPicker::New();
  this->LinePicker->SetTolerance(0.005); //need some fluff
  this->LinePicker->AddPickList( this->LineActor );
  this->LinePicker->PickFromListOn();

  this->RepresentationState = vtkLineRepresentation3::Outside;
  this->AnnotationTextScaleInitialized = false;

  this->RestrictFlag = RestrictNone;

  // Initial creation of the widget, serves to initialize it.
  // Call PlaceWidget() LAST in the constructor, as this method depends on ivar
  // values.
  this->PlaceWidget(bounds);

}

//----------------------------------------------------------------------------
vtkLineRepresentation3::~vtkLineRepresentation3()
{
  if ( this->HandleRepresentation )
  {
    this->HandleRepresentation->Delete();
  }
  if ( this->Point1Representation )
  {
    this->Point1Representation->Delete();
  }
  if ( this->LineHandleRepresentation )
  {
    this->LineHandleRepresentation->Delete();
  }

  this->LineActor->Delete();
  this->LineMapper->Delete();
  this->LineSource->Delete();

  this->Handle->Delete();
  this->HandleMapper->Delete();
  this->HandleGeometry->Delete();

  this->EndPointProperty->Delete();
  this->SelectedEndPointProperty->Delete();
  this->LineProperty->Delete();
  this->SelectedLineProperty->Delete();

  this->BoundingBox->Delete();

  delete [] this->DistanceAnnotationFormat;
  this->DistanceAnnotationFormat = NULL;

  this->TextInput->Delete();
  this->TextMapper->Delete();
  this->TextActor->Delete();
  this->LinePicker->Delete();

}

//----------------------------------------------------------------------
double vtkLineRepresentation3::GetDistance()
{
  return this->Distance;
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::InstantiateHandleRepresentation()
{
  if ( ! this->Point1Representation )
  {
    this->Point1Representation = this->HandleRepresentation->NewInstance();
    this->Point1Representation->ShallowCopy(this->HandleRepresentation);
  }
  if ( ! this->LineHandleRepresentation )
  {
    this->LineHandleRepresentation = this->HandleRepresentation->NewInstance();
    this->LineHandleRepresentation->ShallowCopy(this->HandleRepresentation);
  }
}


//----------------------------------------------------------------------
void vtkLineRepresentation3::SetResolution(int r)
{
  this->LineSource->SetResolution(r);
}

//----------------------------------------------------------------------
int vtkLineRepresentation3::GetResolution()
{
  return this->LineSource->GetResolution();
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::GetPolyData(vtkPolyData *pd)
{
  pd->ShallowCopy(this->LineSource->GetOutput());
}

//-- Set/Get position of the three handles -----------------------------
// Point1
//----------------------------------------------------------------------
void vtkLineRepresentation3::GetPoint1WorldPosition(double pos[3])
{
  this->Point1Representation->GetWorldPosition(pos);
}

double* vtkLineRepresentation3::GetPoint1WorldPosition()
{
  return this->Point1Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::GetPoint1DisplayPosition(double pos[3])
{
  this->Point1Representation->GetDisplayPosition(pos);
}

double* vtkLineRepresentation3::GetPoint1DisplayPosition()
{
  return this->Point1Representation->GetDisplayPosition();
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::SetPoint1WorldPosition(double x[3])
{
  this->Point1Representation->SetWorldPosition(x);
  this->LineSource->SetPoint1(x);
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::SetPoint1DisplayPosition(double x[3])
{
  this->Point1Representation->SetDisplayPosition(x);
  double p[3];
  this->Point1Representation->GetWorldPosition(p);
  this->Point1Representation->SetWorldPosition(p);
}

// Point2 (do not have a point handle representation for this)
//----------------------------------------------------------------------
void vtkLineRepresentation3::GetPoint2WorldPosition(double pos[3])
{
  this->LineSource->GetPoint2(pos);
}

double* vtkLineRepresentation3::GetPoint2WorldPosition()
{
  return this->LineSource->GetPoint2();
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::SetPoint2WorldPosition(double x[3])
{
  this->LineSource->SetPoint2(x);
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::SetRenderer(vtkRenderer* ren)
{
  this->HandleRepresentation->SetRenderer(ren);
  this->Point1Representation->SetRenderer(ren);
  this->LineHandleRepresentation->SetRenderer(ren);
  this->Superclass::SetRenderer(ren);
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::StartWidgetInteraction(double e[2])
{
  // Store the start position
  this->StartEventPosition[0] = e[0];
  this->StartEventPosition[1] = e[1];
  this->StartEventPosition[2] = 0.0;

  // Store the start position
  this->LastEventPosition[0] = e[0];
  this->LastEventPosition[1] = e[1];
  this->LastEventPosition[2] = 0.0;

  // Get the coordinates of the three handles
  this->Point1Representation->GetWorldPosition(this->StartP1);
  this->LineHandleRepresentation->GetWorldPosition(this->StartLineHandle);
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::WidgetInteraction(double e[2])
{
  // Process the motion
  if ( this->InteractionState == vtkLineRepresentation3::OnP1 )
  {
    if (this->RestrictFlag)
    {
      double x[3];
      this->Point1Representation->GetWorldPosition(x);
      for (int i = 0; i < 3; ++i)
      {
        x[i] = (this->RestrictFlag == (i + 1)) ? x[i] : this->StartP1[i];
      }
      this->Point1Representation->SetWorldPosition(x);
    }
  }

  // Store the start position
  this->LastEventPosition[0] = e[0];
  this->LastEventPosition[1] = e[1];
  this->LastEventPosition[2] = 0.0;
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::PlaceWidget(double bds[6])
{
  int i;
  double bounds[6], center[3];

  double placeFactor = this->PlaceFactor;
  this->PlaceFactor = 1.0;
  this->AdjustBounds(bds, bounds, center);
  this->PlaceFactor = placeFactor;

  for (i=0; i<6; i++)
  {
    this->InitialBounds[i] = bounds[i];
  }
  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

  // When PlaceWidget() is invoked, the widget orientation is preserved, but it
  // is allowed to translate and scale. This means it is centered in the
  // bounding box, and the representation scales itself to intersect the sides
  // of the bounding box. Thus we have to determine where Point1 and Point2
  // intersect the bounding box.
  double p1[3], p2[3], r[3], o[3], t, placedP1[3], placedP2[3];
  this->LineSource->GetPoint1(p1);
  this->LineSource->GetPoint2(p2);

  // Okay, this looks really weird, we are shooting rays from OUTSIDE
  // the bounding box back towards it. This is because the IntersectBox()
  // method computes intersections only if the ray originates outside the
  // bounding box.
  r[0] = this->InitialLength * (p1[0] - p2[0]);
  r[1] = this->InitialLength * (p1[1] - p2[1]);
  r[2] = this->InitialLength * (p1[2] - p2[2]);
  o[0] = center[0] - r[0];
  o[1] = center[1] - r[1];
  o[2] = center[2] - r[2];
  vtkBox::IntersectBox(bounds,o,r,placedP1,t);
  this->SetPoint1WorldPosition(placedP1);

  r[0] = this->InitialLength * (p2[0] - p1[0]);
  r[1] = this->InitialLength * (p2[1] - p1[1]);
  r[2] = this->InitialLength * (p2[2] - p1[2]);
  o[0] = center[0] - r[0];
  o[1] = center[1] - r[1];
  o[2] = center[2] - r[2];
  vtkBox::IntersectBox(bounds,o,r,placedP2,t);
  this->SetPoint2WorldPosition(placedP2);

  // Initialize the center point
  this->LineHandleRepresentation->SetWorldPosition(center);

  // Position the handles at the end of the lines
  this->Placed = 1;
  this->ValidPick = 1;
  this->BuildRepresentation();
}


//----------------------------------------------------------------------------
int vtkLineRepresentation3::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
  // Check if we are on end points. Use the handles to determine this.
  int p1State = this->Point1Representation->ComputeInteractionState(X,Y,0);
  if ( p1State == vtkHandleRepresentation::Nearby )
  {
    this->InteractionState = vtkLineRepresentation3::OnP1;
    this->SetRepresentationState(vtkLineRepresentation3::OnP1);
  }
  else
  {
    this->InteractionState = vtkLineRepresentation3::Outside;
  }
  return this->InteractionState;
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::SetRepresentationState(int state)
{
  if (this->RepresentationState == state)
  {
    return;
  }

  state = (state < vtkLineRepresentation3::Outside ?
           vtkLineRepresentation3::Outside :
           (state > vtkLineRepresentation3::Scaling ?
            vtkLineRepresentation3::Scaling : state));

  this->RepresentationState = state;
  this->Modified();

  if ( state == vtkLineRepresentation3::Outside )
  {
    this->HighlightPoint(0,0);
    this->HighlightPoint(1,0);
    this->HighlightLine(0);
  }
  else if ( state == vtkLineRepresentation3::OnP1 )
  {
    this->HighlightPoint(0,1);
    this->HighlightPoint(1,0);
    this->HighlightLine(0);
  }
  else
  {
    this->HighlightPoint(0,1);
    this->HighlightPoint(1,1);
    this->HighlightLine(1);
  }
}

//----------------------------------------------------------------------
double *vtkLineRepresentation3::GetBounds()
{
  this->BuildRepresentation();
  this->BoundingBox->SetBounds(this->LineActor->GetBounds());
  this->BoundingBox->AddBounds(this->Handle->GetBounds());

  return this->BoundingBox->GetBounds();
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::CreateDefaultProperties()
{
  // Endpoint properties
  this->EndPointProperty = vtkProperty::New();
  this->EndPointProperty->SetColor(1,1,1);

  this->SelectedEndPointProperty = vtkProperty::New();
  this->SelectedEndPointProperty->SetColor(0,1,0);

  // Line properties
  this->LineProperty = vtkProperty::New();
  this->LineProperty->SetAmbient(1.0);
  this->LineProperty->SetAmbientColor(1.0,1.0,1.0);
  this->LineProperty->SetLineWidth(2.0);

  this->SelectedLineProperty = vtkProperty::New();
  this->SelectedLineProperty->SetAmbient(1.0);
  this->SelectedLineProperty->SetAmbientColor(0.0,1.0,0.0);
  this->SelectedLineProperty->SetLineWidth(2.0);
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::SizeHandles()
{
  // The SizeHandles() method depends on the LastPickPosition data member.
  double radius = this->vtkWidgetRepresentation::SizeHandlesInPixels(1.35,this->LineSource->GetPoint1());
  static_cast<vtkSphereSource*>(this->HandleGeometry)->SetRadius(radius);
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::BuildRepresentation()
{
  // Rebuild only if necessary
  if ( this->GetMTime() > this->BuildTime ||
       this->Point1Representation->GetMTime() > this->BuildTime ||
       this->LineHandleRepresentation->GetMTime() > this->BuildTime ||
       (this->Renderer && this->Renderer->GetVTKWindow() &&
        (this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime ||
        this->Renderer->GetActiveCamera()->GetMTime() > this->BuildTime)) )
  {
    if ( ! this->InitializedDisplayPosition && this->Renderer )
    {
      this->SetPoint1WorldPosition(this->LineSource->GetPoint1());
      this->SetPoint2WorldPosition(this->LineSource->GetPoint2());
      this->ValidPick = 1;
      this->InitializedDisplayPosition = 1;
    }

    // Make sure that tolerance is consistent between handles and this representation
    this->Point1Representation->SetTolerance(this->Tolerance);
    this->LineHandleRepresentation->SetTolerance(this->Tolerance);

    // Retrieve end point information
    double x1[3], x2[3];
    this->GetPoint1WorldPosition(x1);

    this->LineSource->SetPoint1(x1);
    static_cast<vtkSphereSource*>(this->HandleGeometry)->SetCenter(x1);

    this->GetPoint2WorldPosition(x2);
    this->LineSource->SetPoint2(x2);

    this->Distance = sqrt(vtkMath::Distance2BetweenPoints( x1, x2 ));

    // Place the DistanceAnnotation right in between the two points.
    double x[3] = { (x1[0] + x2[0])/2.0,
                    (x1[1] + x2[1])/2.0,
                    (x1[2] + x2[2])/2.0 };
    char string[512];
    sprintf(string, this->DistanceAnnotationFormat, this->Distance);
    this->TextInput->SetText( string );
    this->TextActor->SetPosition( x );
    if (this->Renderer)
    {
      this->TextActor->SetCamera( this->Renderer->GetActiveCamera() );
    }

    if (!this->AnnotationTextScaleInitialized)
    {
      // If a font size hasn't been specified by the user, scale the text
      // (font size) according to the length of the line widget.
      this->TextActor->SetScale(
          this->Distance/10.0, this->Distance/10.0, this->Distance/10.0 );
    }

    this->SizeHandles();
    this->BuildTime.Modified();
  }
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::HighlightPoint(int ptId, int highlight)
{
  if ( ptId == 0 )
  {
    if ( highlight )
    {
      this->Handle->SetProperty(this->SelectedEndPointProperty);
      this->Point1Representation->SetSelectedProperty(this->SelectedEndPointProperty);
    }
    else
    {
      this->Handle->SetProperty(this->EndPointProperty);
      this->Point1Representation->SetProperty(this->EndPointProperty);
    }
  }
  else
  {
    if ( highlight )
    {
      this->LineHandleRepresentation->SetSelectedProperty(this->SelectedEndPointProperty);
    }
    else
    {
      this->LineHandleRepresentation->SetProperty(this->EndPointProperty);
    }
  }
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::HighlightLine(int highlight)
{
  if ( highlight )
  {
    this->LineActor->SetProperty(this->SelectedLineProperty);
  }
  else
  {
    this->LineActor->SetProperty(this->LineProperty);
  }
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::SetLineColor(double r, double g, double b)
{
  if(this->GetLineProperty())
  {
    this->GetLineProperty()->SetColor(r, g, b);
  }
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::ClampPosition(double x[3])
{
  for (int i=0; i<3; i++)
  {
    if ( x[i] < this->InitialBounds[2*i] )
    {
      x[i] = this->InitialBounds[2*i];
    }
    if ( x[i] > this->InitialBounds[2*i+1] )
    {
      x[i] = this->InitialBounds[2*i+1];
    }
  }
}

//----------------------------------------------------------------------------
int vtkLineRepresentation3::InBounds(double x[3])
{
  for (int i=0; i<3; i++)
  {
    if ( x[i] < this->InitialBounds[2*i] ||
         x[i] > this->InitialBounds[2*i+1] )
    {
      return 0;
    }
  }
  return 1;
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::GetActors(vtkPropCollection *pc)
{
  this->LineActor->GetActors(pc);
  this->Handle->GetActors(pc);
  this->TextActor->GetActors(pc);
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::ReleaseGraphicsResources(vtkWindow *w)
{
  this->LineActor->ReleaseGraphicsResources(w);
  this->Handle->ReleaseGraphicsResources(w);
  this->TextActor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------------
int vtkLineRepresentation3::RenderOpaqueGeometry(vtkViewport *v)
{
  int count=0;
  this->BuildRepresentation();
  count += this->LineActor->RenderOpaqueGeometry(v);
  count += this->Handle->RenderOpaqueGeometry(v);
  if (this->DistanceAnnotationVisibility)
  {
    count += this->TextActor->RenderOpaqueGeometry(v);
  }

  return count;
}

//----------------------------------------------------------------------------
int vtkLineRepresentation3::RenderTranslucentPolygonalGeometry(vtkViewport *v)
{
  int count=0;
  this->BuildRepresentation();
  count += this->LineActor->RenderTranslucentPolygonalGeometry(v);
  count += this->Handle->RenderTranslucentPolygonalGeometry(v);
  if (this->DistanceAnnotationVisibility)
  {
    count += this->TextActor->RenderTranslucentPolygonalGeometry(v);
  }

  return count;
}

//----------------------------------------------------------------------------
int vtkLineRepresentation3::HasTranslucentPolygonalGeometry()
{
  int result=0;
  this->BuildRepresentation();
  result |= this->LineActor->HasTranslucentPolygonalGeometry();
  result |= this->Handle->HasTranslucentPolygonalGeometry();
  if (this->DistanceAnnotationVisibility)
  {
    result |= this->TextActor->HasTranslucentPolygonalGeometry();
  }

  return result;
}

//----------------------------------------------------------------------------
vtkMTimeType vtkLineRepresentation3::GetMTime()
{
  vtkMTimeType mTime=this->Superclass::GetMTime();
  vtkMTimeType mTime2=this->Point1Representation->GetMTime();
  mTime = ( mTime2 > mTime ? mTime2 : mTime );
  mTime = ( mTime2 > mTime ? mTime2 : mTime );
  mTime2=this->LineHandleRepresentation->GetMTime();
  mTime = ( mTime2 > mTime ? mTime2 : mTime );

  return mTime;
}

//----------------------------------------------------------------------
void vtkLineRepresentation3::SetDistanceAnnotationScale( double scale[3] )
{
  this->TextActor->SetScale( scale );
  this->AnnotationTextScaleInitialized = true;
}

//----------------------------------------------------------------------
double * vtkLineRepresentation3::GetDistanceAnnotationScale()
{
  return this->TextActor->GetScale();
}

//----------------------------------------------------------------------------
vtkProperty * vtkLineRepresentation3::GetDistanceAnnotationProperty()
{
  return this->TextActor->GetProperty();
}

//----------------------------------------------------------------------------
void vtkLineRepresentation3::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->LineProperty )
  {
    os << indent << "Line Property: " << this->LineProperty << "\n";
  }
  else
  {
    os << indent << "Line Property: (none)\n";
  }
  if ( this->SelectedLineProperty )
  {
    os << indent << "Selected Line Property: "
       << this->SelectedLineProperty << "\n";
  }
  else
  {
    os << indent << "Selected Line Property: (none)\n";
  }

  if ( this->EndPointProperty )
  {
    os << indent << "End Point Property: " << this->EndPointProperty << "\n";
  }
  else
  {
    os << indent << "End Point Property: (none)\n";
  }
  if ( this->SelectedEndPointProperty )
  {
    os << indent << "Selected End Point Property: " << this->SelectedEndPointProperty << "\n";
  }
  else
  {
    os << indent << "Selected End Point Property: (none)\n";
  }

  os << indent << "Tolerance: " << this->Tolerance << "\n";

  os << indent << "Constrain To Bounds: "
     << (this->ClampToBounds ? "On\n" : "Off\n");

  int res = this->LineSource->GetResolution();
  double *pt1 = this->LineSource->GetPoint1();
  double *pt2 = this->LineSource->GetPoint2();

  os << indent << "Resolution: " << res << "\n";
  os << indent << "Point 1: (" << pt1[0] << ", "
                               << pt1[1] << ", "
                               << pt1[2] << ")\n";
  os << indent << "Point 2: (" << pt2[0] << ", "
                               << pt2[1] << ", "
                               << pt2[2] << ")\n";

  os << indent << "Point1 Representation: ";
  this->Point1Representation->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Line Handle Representation: ";
  this->LineHandleRepresentation->PrintSelf(os,indent.GetNextIndent());

  os << indent << "Representation State: " << this->RepresentationState << "\n";
  os << indent << "Restring flag: ";
  switch (this->RestrictFlag)
  {
    case RestrictNone:
      os << "RestrictNone";
      break;
    case RestrictToX:
      os << "RestrictToX";
      break;
    case RestrictToY:
      os << "RestrictToY";
      break;
    case RestrictToZ:
      os << "RestrictToZ";
      break;
    default:
      os << "unexpected value: " << this->RestrictFlag;
      break;
  }
  os << "\n";

  os << indent << "DistanceAnnotationVisibility: ";
  if ( this->DistanceAnnotationVisibility )
  {
    os << this->DistanceAnnotationVisibility << "\n";
  }
  else
  {
    os << "(none)\n";
  }

  os << indent << "DistanceAnnotationFormat: ";
  if ( this->DistanceAnnotationFormat )
  {
    os << this->DistanceAnnotationFormat << "\n";
  }
  else
  {
    os << "(none)\n";
  }

  os << indent << "TextActor: ";
  if ( this->TextActor )
  {
    os << this->TextActor << "\n";
  }
  else
  {
    os << "(none)\n";
  }

  // this->InteractionState is printed in superclass
  // this is commented to avoid PrintSelf errors
}
