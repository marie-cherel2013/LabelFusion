/*=========================================================================

  Program:   Diffusion Applications
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/DiffusionApplications/ResampleDTI/itkDiffusionTensor3DResample.txx $
  Language:  C++
  Date:      $Date: 2010-04-29 11:58:49 -0400 (Thu, 29 Apr 2010) $
  Version:   $Revision: 13073 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkWeightedMajorityVotingFilter_hxx
#define __itkWeightedMajorityVotingFilter_hxx

#include "itkWeightedMajorityVotingFilter.h"

namespace itk
{

   template <class TInput, class TOutput>
   WeightedMajorityVotingFilter <TInput, TOutput>
   ::WeightedMajorityVotingFilter()
   {
     this->SetNumberOfRequiredInputs( 1 );

     m_OutputSpacing.Fill( 1.0 );
     m_OutputOrigin.Fill( 0.0 );
     m_OutputDirection.SetIdentity();
     m_OutputSize.Fill( 0 );

     m_ComputingWeights = true;
     m_RadiusValue = 1; 

   }


   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput>
   ::SetInput(InputImagePointerType inputImage)
   {
     this -> Superclass::SetInput(0, inputImage);
   }

   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput> 
   ::SetComputingWeightsOff()
   {
      m_ComputingWeights = false;
   }

   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput>
   ::SetComputingWeightsOn()
   {
      m_ComputingWeights = true;
   }

   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput>
   ::SetRadiusValueInMillimetersOn ()
   {
     m_radiusValueInMillimeters = true;
   }

   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput>
   ::SetRadiusValueInMillimetersOff ()
   {
     m_radiusValueInMillimeters = false;
   }
   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput>
   ::SetOutputDirectory(std::string outputDirectory)
   {
      m_outputDirectory = outputDirectory;
   }


   template <class TInput, class TOutput>
   void 
   WeightedMajorityVotingFilter <TInput, TOutput>
   ::AddAtlas( std::string name, InputImagePointerType image, InputImagePointerType labels )
   {
      AtlasType atlas;

      atlas.name = name;
      atlas.image = image;
      atlas.labels = labels; 
      atlas.weights = NULL; 

      m_atlasPopulation.push_back(atlas);
   }

   template <class TInput, class TOutput>
   void
   WeightedMajorityVotingFilter<TInput, TOutput>
   ::SetOutputParametersFromImage( InputImagePointerType Image )
   {
     typename TOutput::RegionType region;
     region = Image->GetLargestPossibleRegion();
     
     m_OutputSize = region.GetSize();
     m_OutputSpacing = Image->GetSpacing();
     m_OutputDirection = Image->GetDirection();
     m_OutputOrigin = Image->GetOrigin();
   }

   template <class TInput, class TOutput>
   void
   WeightedMajorityVotingFilter<TInput, TOutput>
   ::BeforeThreadedGenerateData()
   {
      m_inputImage = const_cast <TInput*> (this->GetInput(0));

      AtlasVectorIteratorType it_atlas;
      it_atlas=m_atlasPopulation.begin(); 

      if (m_ComputingWeights)
      {
         typedef LocalNormalizedCorrelationWeightFilter < TInput, TInput >       LocalWeightFilterType;
         typedef typename LocalWeightFilterType::Pointer                         LocalWeightFilterPointerType;

         LocalWeightFilterPointerType localWeightFilter = LocalWeightFilterType::New();

         localWeightFilter->SetNumberOfThreads(1);
         localWeightFilter->SetInput(0, m_inputImage);
         localWeightFilter->SetRadiusValueInMillimetersOn();
         localWeightFilter->SetRadiusValue(m_RadiusValue);
         localWeightFilter->SetOutputParametersFromImage(m_inputImage);


         typedef ThresholdImageFilter < TInput >                                 ThresholdFilterType;
         typedef typename ThresholdFilterType::Pointer                           ThresholdFilterPointerType;

         ThresholdFilterPointerType thresholdFilter = ThresholdFilterType::New();

         thresholdFilter->ThresholdOutside(0, 1);
         thresholdFilter->SetOutsideValue(0);

         typedef ImageFileWriter < TInput >                                       FileWriterType; 
         typedef typename FileWriterType::Pointer                                 FileWriterPointerType;

         FileWriterPointerType writer = FileWriterType::New(); 
         std::string name;

         while( it_atlas!=m_atlasPopulation.end() )
         { 
            localWeightFilter->SetInput(1, (*it_atlas).image);
            localWeightFilter->BeforeThreadedGenerateData();
            localWeightFilter->Update();

            thresholdFilter->SetInput(localWeightFilter->GetOutput());
            thresholdFilter->Update();

            name = m_outputDirectory + "/" + (*it_atlas).name + "_weights.nrrd";

            writer->SetInput(thresholdFilter->GetOutput());
            writer->SetFileName(name);
            writer->Update(); 


            (*it_atlas).weights = thresholdFilter->GetOutput();
            ++it_atlas;
         } 
      }

      else
      {
         InputImagePointerType weights = InputImageType::New(); 
         typename TOutput::RegionType region;
         region = ((*it_atlas).labels)->GetLargestPossibleRegion();     
         weights -> SetRegions( region );
         weights -> Allocate();
         weights -> FillBuffer(1);  

         while( it_atlas!=m_atlasPopulation.end() )
         {
            (*it_atlas).weights = weights;    
            ++it_atlas;
         }
      }
   }


   template <class TInput, class TOutput>
   void WeightedMajorityVotingFilter <TInput, TOutput>
   ::ThreadedGenerateData (const OutputImageRegionType &outputRegionForThread, ThreadIdType itkNotUsed(threadId))
   {
      // Output
      OutputImagePointerType     outputImagePtr = this->GetOutput( 0 );
      IteratorType               it( outputImagePtr, outputRegionForThread );
      OutputImageIndexType       index;

      typedef MinimumMaximumImageCalculator <TInput>     MinMaxCalculatorType;
      typedef typename MinMaxCalculatorType::Pointer     MinMaxCalculatorPointerType;
      MinMaxCalculatorPointerType minMaxCalculator = MinMaxCalculatorType::New(); 

      std::vector<double> labels(4,0.0);      

      AtlasType atlas;

      int label;
      double weight;

      double majorityLabel; 

      long progress;



      it.GoToBegin();  
      while( !it.IsAtEnd() )
      { 
          std::fill(labels.begin(), labels.end(), 0.0);

          index = it.GetIndex();

          AtlasVectorIteratorType it_atlas;



          it_atlas=m_atlasPopulation.begin();  
          while( it_atlas!=m_atlasPopulation.end() )
          { 
            atlas = *(it_atlas);

            label = (atlas.labels) -> GetPixel( index );
            weight = (atlas.weights) -> GetPixel( index );

            labels[ label ] = labels[ label ] + weight;
            
            ++it_atlas;
          }

         if (m_inputImage->GetPixel(index)==0)
         {
            majorityLabel=0;
         }

         else
         {
            majorityLabel = std::distance(labels.begin(), std::max_element(labels.begin(), labels.end())); 
         }

         if ( progress % 10000 == 0 )
         {
            std::cout<<"At index:"<<index<<" labels = "<<labels[0]<<" "<<labels[1]<<" "<<labels[2]<<" "<<labels[3]<<" majority label = "<<majorityLabel<<std::endl;
         } 

         it.Set(majorityLabel);

         ++it;
         ++progress;
     }
   }

   template <class TInput, class TOutput>
   void
   WeightedMajorityVotingFilter<TInput, TOutput>
   ::GenerateOutputInformation()
   {
     Superclass::GenerateOutputInformation();
     OutputImagePointerType outputPointer = this->GetOutput( 0 );

     if( !outputPointer )
     {
       return;
     }

     OutputImageRegionType outputRegion;
     outputRegion.SetSize( m_OutputSize );
     OutputImageIndexType index;
     index.Fill( 0 );

     outputRegion.SetIndex( index );
     outputPointer->SetRegions( outputRegion );
     outputPointer->SetSpacing( m_OutputSpacing );
     outputPointer->SetOrigin( m_OutputOrigin );
     outputPointer->SetDirection( m_OutputDirection );

     return;
   }


   template <class TInput, class TOutput>
   void
   WeightedMajorityVotingFilter<TInput, TOutput>
   ::GenerateInputRequestedRegion()
   {
     Superclass::GenerateInputRequestedRegion();

     if( !(this->GetInput()) )
     {
       return;
     }

     // get pointers to the input and output
     InputImagePointerType inputPointer = const_cast<TInput *>( this->GetInput() );

     // Request the entire input image
     typename TInput::RegionType inputRegion;
     inputRegion = inputPointer->GetLargestPossibleRegion();
     inputPointer->SetRequestedRegion( inputRegion );

     return;
   }


} // end namespace itk
#endif

