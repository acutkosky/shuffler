#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <string>
#include <cstdio>
#include <vector>
#include <ctime>

#define IN_MEMORY_SIZE 100000
#define BUFFER_LINE 1000
#define BRANCHING 15

using namespace std;

string getFileNameFromPath(string path) {
  std::size_t lastSlash = path.rfind('/');
  string name = path.substr(lastSlash+1, path.length());
  return name;
}

int splitFile(string sourceFile, vector<string> destFiles) {
  string line;
  int lineCount = 0;
  ifstream sourceStream(sourceFile);
  if (!sourceStream.is_open()) {
    return -1;
  }


  ofstream* destStreams = new ofstream[destFiles.size()];

  

  string* outbuffers = new string[destFiles.size()];
  int* outbuffer_lengths = new int[destFiles.size()];
  int numDests = destFiles.size();

  for(int i=0; i<destFiles.size(); i++) {
    destStreams[i].open(destFiles[i]);
    if(!destStreams[i].is_open())
      return -1;
    outbuffers[i].clear();
    outbuffer_lengths[i] = 0;
  }

  while (getline(sourceStream, line)) {
    lineCount ++;
    int destination = rand() % numDests;
    outbuffers[destination] += line + '\n';
    outbuffer_lengths[destination] ++;
    if(outbuffer_lengths[destination] > BUFFER_LINE) {
      destStreams[destination] << outbuffers[destination];
      outbuffers[destination].clear();
      outbuffer_lengths[destination] = 0;
      
    }
  }

  for(int i=0; i<numDests; i++) {
    if(outbuffer_lengths[i] > 0) {
      destStreams[i] << outbuffers[i];
    }
    destStreams[i].close();
  }

  sourceStream.close();
  delete [] destStreams;
  delete [] outbuffers;
  delete [] outbuffer_lengths;

  return lineCount;

}

int mergeFiles(string destFile, vector<string> sourceFiles) {
  string line;
  ofstream destStream(destFile);
  string buffer;
  int buffer_length = 0;

  if (!destStream.is_open())
    return 1;

  for(int i=0; i<sourceFiles.size(); i++) {
    ifstream sourceStream(sourceFiles[i]);
    if(!sourceStream.is_open())
      return 1;
    while(getline(sourceStream, line)) {
      buffer += line + '\n';
      buffer_length ++;
      if(buffer_length > BUFFER_LINE) {
        destStream << buffer;
        buffer_length = 0;
        buffer.clear();
      }
    }
    sourceStream.close();
    if (buffer_length > 0) {
      destStream << buffer;
    }
    buffer_length = 0;
    buffer.clear();
  }
  destStream.close();

  return 0;
}

int inMemoryShuffle(string toShuffle, string destination) {
  vector<string> linesInFile;
  string line;

  ifstream filePointer(toShuffle);
  if (!filePointer.is_open())
    return 1;

  while (getline(filePointer, line)) {
    linesInFile.push_back(line);
  }
  filePointer.close();

  int length = linesInFile.size();
  int* ordering = new int[length];

  //generate a random shuffle
  for (int i=0; i<length; i++) {
    ordering[i] = i;
  }
  for (int i=0; i<length; i++) {
    int swapIndex = rand()%(length-i);
    int tmp = ordering[i];
    ordering[i] = ordering[i+swapIndex];
    ordering[i+swapIndex] = tmp;
  }

  ofstream destStream(destination);
  if(!destStream.is_open())
    return 1;
  for(int i=0; i<length; i++) {
    destStream << linesInFile[ordering[i]] << '\n';
  }
  destStream.close();

  return 0;
}

int copyFile(string fileToCopy, string destFile) {
  string line;
  ofstream destStream(destFile);

  if (!destStream.is_open())
    return 1;

  ifstream sourceStream(fileToCopy);
  if (!sourceStream.is_open())
    return 1;
  while (getline(sourceStream, line)) {
    destStream << line << '\n';
  }
  sourceStream.close();
  return 0;
}

int mergeShuffle(string fileToShuffle, string destination, int branching, int depth) {

  vector<string> splitFiles;
  vector<string> shuffledFiles;
  char buf[10];
  for(int i=0;i<10;i++) {
    buf[i] = 0;
  }

  string tmp = string("/tmp/");
  sprintf(buf , "%d", rand());
  tmp += buf;


  for(int i=0; i<branching; i++) {
    sprintf(buf , "%d", i);
    splitFiles.push_back(tmp + getFileNameFromPath(fileToShuffle) + "-split" + buf);
    shuffledFiles.push_back(tmp + getFileNameFromPath(fileToShuffle) + "-shuffled"+buf);
  }

  int lines = splitFile(fileToShuffle, splitFiles);

  if (lines < 0)
    return 1;
  if (lines <= branching * IN_MEMORY_SIZE) {
    for(int i=0; i< branching; i++) {
      inMemoryShuffle(splitFiles[i], shuffledFiles[i]);
      remove(splitFiles[i].c_str());
    }
  } else {
    for(int i=0; i< branching; i++) {
        if(mergeShuffle(splitFiles[i], shuffledFiles[i], branching, depth+1))
          return 1;
        remove(splitFiles[i].c_str());
    }
  }

  if(mergeFiles(destination, shuffledFiles))
    return 1;


  for(int i=0; i<branching; i++) {
    remove(shuffledFiles[i].c_str());
  }

  return 0;
}

void printUsage() {
  cout<<"Usage: shuffler fileToShuffle outputFile\n";
}

int main(int argc, char* argv[]) {
  srand (time(NULL));

  if (argc<3) {
    printUsage();
    return 0;
  }

  return mergeShuffle(string(argv[1]), string(argv[2]), BRANCHING, 1);
}
