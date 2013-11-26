/*=========================================================================

  Program:   Diffusion Applications
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/DiffusionApplications/ResampleDTI/itkDiffusionTensor3DResample.h $
  Language:  C++
  Date:      $Date: 2010-06-28 07:45:15 -0400 (Mon, 28 Jun 2010) $
  Version:   $Revision: 13964 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkWeightedMajorityVotingFilter_h
#define __itkWeightedMajorityVotingFilter_h

#include "itkLocalNormalizedCorrelationWeightFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkThresholdImageFilter.h"

//#include <itkObject.h>
#include "itkImageToImageFilter.h"
#include "Atlas.h"

#include <vector>
#include <iterator>
#include <string>



namespace itk
{

   template <class TInput, class TOutput>
   class WeightedMajorityVotingFilter : public ImageToImageFilter
      < TInput, TOutput >
   {

   public:

     typedef ImageToImageFilter < TInput, TOutput >                   Superclass;
     typedef WeightedMajorityVotingFilter                             Self;
     typedef SmartPointer<Self>                                       Pointer;
     typedef SmartPointer<const Self>                                 ConstPointer;

     typedef TInput                                                   InputImageType;
     typedef TOutput                                                  OutputImageType;

     typedef typename TInput::Pointer                                 InputImagePointerType;
     typedef typename TOutput::Pointer                                OutputImagePointerType;

     typedef typename TInput::RegionType                              InputImageRegionType;
     typedef typename TOutput::RegionType                             OutputImageRegionType;

     typedef typename TInput::SizeType                                InputImageSizeType;
     typedef typename TOutput::SizeType                               OutputImageSizeType;

     typedef typename TInput::IndexType                               InputImageIndexType;
     typedef typename TOutput::IndexType                              OutputImageIndexType;

     typedef typename TInput::PixelType                               InputImagePixelType;
     typedef typename TOutput::PixelType                              OutputImagePixelType;

     typedef Atlas<InputImagePointerType>                             AtlasType;
     typedef typename std::vector<AtlasType>                          AtlasVectorType;
     typedef typename AtlasVectorType::iterator                       AtlasVectorIteratorType;




     typedef ImageRegionIterator<TOutput>                             IteratorType;

     itkNewMacro( Self );

/// Set the input image
  void SetInput( InputImagePointerType inputImage );
  void AddAtlas( std::string name, InputImagePointerType image, InputImagePointerType labels );

  itkSetMacro( OutputOrigin, typename TOutput::PointType );
  itkSetMacro( OutputSpacing, typename TOutput::SpacingType );
  itkSetMacro( OutputSize, typename TOutput::SizeType );
  itkSetMacro( OutputDirection, typename TOutput::DirectionType );

  itkGetMacro( OutputOrigin, typename TOutput::PointType );
  itkGetMacro( OutputSpacing, typename TOutput::SpacingType );
  itkGetMacro( OutputSize, typename TOutput::SizeType );
  itkGetMacro( OutputDirection, typename TOutput::DirectionType );

  void SetComputingWeightsOn();
  void SetComputingWeightsOff();

  void SetRadiusValueInMillimetersOn ();
  void SetRadiusValueInMillimetersOff ();

  itkSetMacro( RadiusValue, double );

  void SetOutputDirectory(std::string outputDirectory);

///Set the output parameters (size, spacing, origin, orientation) from a reference image
  void SetOutputParametersFromImage( InputImagePointerType Image );

//protected:

  WeightedMajorityVotingFilter();
  void BeforeThreadedGenerateData();
  void ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId );
  void GenerateOutputInformation();
  void GenerateInputRequestedRegion();

private:

  std::string                          m_outputDirectory;

  bool                                 m_radiusValueInMillimeters;
  double                               m_RadiusValue;        

  bool                                 m_ComputingWeights;

  AtlasVectorType                      m_atlasPopulation;
  InputImagePointerType                m_inputImage;
  typename TOutput::PointType          m_OutputOrigin;
  typename TOutput::SpacingType        m_OutputSpacing;
  typename TOutput::SizeType           m_OutputSize;
  typename TOutput::DirectionType      m_OutputDirection;


};

} // end namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWeightedMajorityVotingFilter.hxx"
#endif

#endif
