// General libraries // 
#include <iostream>
#include <string>
#include <vector>
#include <iterator>

// ITK librairies //
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWeightedMajorityVotingFilter.h"

// Types // 
typedef itk::Image<float, 3>                    FloatImageType;
typedef itk::ImageFileReader<FloatImageType>    ReaderType; 
typedef itk::ImageFileWriter<FloatImageType>    WriterType; 

int main(int argc, char* argv[])
{
   std::string neo = "neo-0106-1";
   std::string outputDirectory = "/work/mcherel/project/tests/" + neo + "/majorityVoting/withoutWeight";
   std::string array[] = {"neo-0121-1", "neo-0203-1-1", "neo-0321-1", "NeoBrainS12-1", "NeoBrainS12-2", "T0244-1-1", "T0274-1-1", "T0287-1-1", "T0295-1-2"};
   std::vector<std::string> atlasPopulation(array, array + 9);

   typedef itk::WeightedMajorityVotingFilter< FloatImageType, FloatImageType > MajorityVotingFilterType;
   MajorityVotingFilterType::Pointer majorityVotingFilter = MajorityVotingFilterType::New();

   majorityVotingFilter->SetNumberOfThreads(1);
   majorityVotingFilter->SetComputingWeightsOff();
   //majorityVotingFilter->SetRadiusValueInMillimetersOn();
   //majorityVotingFilter->SetRadiusValue(atoi(argv[1]));
   majorityVotingFilter->SetOutputDirectory(outputDirectory); 

   // Image //
   std::string imageNeo = "/work/mcherel/project/atlasPopulation/atlases/" + neo + "/T2-stripped.nrrd";
	ReaderType::Pointer imageNeo_reader = ReaderType::New(); 
	imageNeo_reader->SetFileName(imageNeo.c_str());
   imageNeo_reader->Update();

   majorityVotingFilter->SetInput(imageNeo_reader->GetOutput());
   majorityVotingFilter->SetOutputParametersFromImage(imageNeo_reader->GetOutput());

   // Atlas Population // 
   std::vector<std::string>::iterator it; 
   for (it=atlasPopulation.begin(); it!=atlasPopulation.end(); ++it)
   {
      std::string atlas = *it; 

      std::string image = "/work/mcherel/project/tests/" + neo + "/atlasPopulation/Reg_" + atlas + "/" + atlas + "_to_" + neo + "-T2.nrrd"; 
      std::string labels = "/work/mcherel/project/tests/" + neo + "/atlasPopulation/Reg_" + atlas + "/" + atlas + "_to_" + neo + "-seg.nrrd"; 

      ReaderType::Pointer labels_reader = ReaderType::New(); 
	   labels_reader->SetFileName(labels.c_str()); 
      labels_reader->Update();

	   ReaderType::Pointer image_reader = ReaderType::New(); 
	   image_reader->SetFileName(image.c_str()); 
      image_reader->Update();

      majorityVotingFilter->AddAtlas(atlas, image_reader->GetOutput(), labels_reader->GetOutput());
   }



   majorityVotingFilter->Update();

   // Weighted Image // 
   std::string name = outputDirectory + "/seg_withoutWeight.nrrd";
	WriterType::Pointer writer = WriterType::New(); 
	writer->SetInput(majorityVotingFilter->GetOutput()); 
	writer->SetFileName(name);
	writer->Update();

}
