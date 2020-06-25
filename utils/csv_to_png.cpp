#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <vector>

using namespace std;

float *normalize_percentile(const float *a, int nx, int nz, float percentile) {
  float max_val = -std::numeric_limits<float>::max();
  vector<float> values;
  float *mod_img = new float[nx * nz];
  values.push_back(0);
  for (int i = 0; i < nx * nz; i++) {
    if (a[i] != 0) {
      values.push_back(fabs(a[i]));
    }
  }
  std::sort(values.begin(), values.end());
  int index = ((percentile / 100.0) * (values.size() - 1));
  if (index < 0) {
    index = 0;
  }
  max_val = values[index];
  for (int i = 0; i < nx * nz; i++) {
    if (fabs(a[i]) > max_val) {
      if (a[i] > 0) {
        mod_img[i] = max_val;
      } else {
        mod_img[i] = -max_val;
      }
    } else {
      mod_img[i] = a[i];
    }
  }
  return mod_img;
}

void GridToPNG(const float *ptr, const int nz, const int nx, char *filename,
               float maxV, float minV) {
  string title = "velocity";
  const cv::Mat data(nz, nx, CV_32FC1, const_cast<float *>(ptr));
  cv::Mat data_displayed(nz, nx, CV_8UC1);
  cv::Mat data_scaled = (data - minV) / (maxV - minV);
  data_scaled.convertTo(data_displayed, CV_8UC1, 255.0, 0);
  //    cv::applyColorMap(data_displayed, data_displayed, cv::COLORMAP_BONE);
  cv::imwrite(filename, data_displayed);
}

/*function... might want it in some class?*/
int getdir(string dir, vector<string> &files) {
  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return 0;
}

bool hasEnding(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  } else {
    return false;
  }
}

vector<string> filter_files(vector<string> &files, string &extension) {
  vector<string> filtered;
  for (unsigned int i = 0; i < files.size(); i++) {
    if (hasEnding(files[i], extension)) {
      filtered.push_back(files[i]);
    }
  }
  return filtered;
}

void ReadCsv(float *mat, uint nz, uint nx, string filename) {
  std::ifstream in(filename);
  string line, word;
  getline(in, line);
  for (uint row = 0; row < nz; row++) {
    getline(in, line, '\n');
    stringstream s(line);
    for (uint col = 0; col < nx; col++) {
      getline(s, word, ',');
      mat[row * nx + col] = stod(word);
    }
  }
}

void ReadCsvHeader(uint *nz, uint *nx, string filename) {
  std::ifstream in(filename);
  char v;
  int n;
  in >> n;
  *nx = n;
  in >> v;
  in >> n;
  *nz = n;
  in >> v;
}

int main(int argc, char *argv[]) {
  string dir = string(".");
  vector<string> files = vector<string>();
  string extension = ".csv";
  getdir(dir, files);
  files = filter_files(files, extension);
  uint nz, nx;
  if (files.size() == 0) {
    return 0;
  }
  cout << "Files to be processed : " << endl;
  for (unsigned int i = 0; i < files.size(); i++) {
    cout << "\t" << files[i] << endl;
  }
  ReadCsvHeader(&nz, &nx, files[0]);
  uint nx_nz = nx * nz;
  uint nt = files.size();
  cout << "Grid dimensions : nx = " << nx << " nz = " << nz << endl;
  float *matrix = new float[nt * nx_nz];
  for (unsigned int i = 0; i < files.size(); i++) {
    cout << "Reading csv : " << files[i] << endl;
    ReadCsv(matrix + i * nx_nz, nz, nx, files[i]);
  }
  float *matrix_percentiled =
      normalize_percentile(matrix, nx_nz, nt, stof(argv[1]));
  float minV =
      *min_element(matrix_percentiled, matrix_percentiled + nt * nx_nz);
  float maxV =
      *max_element(matrix_percentiled, matrix_percentiled + nt * nx_nz);
  for (unsigned int i = 0; i < files.size(); i++) {
    string s = files[i];
    s.replace(s.end() - 4, s.end(), ".png");
    GridToPNG(matrix_percentiled + i * nx_nz, nz, nx, (char *)s.c_str(), maxV,
              minV);
  }
  return 0;
}