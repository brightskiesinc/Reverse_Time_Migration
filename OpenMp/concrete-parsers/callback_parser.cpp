//
// Created by amr on 25/01/2020.
//

#include "callback_parser.h"
#include <concrete-callbacks/csv_writer.h>
#include <concrete-callbacks/image_writer.h>
#include <concrete-callbacks/norm_writer.h>
#include <concrete-callbacks/segy_writer.h>
#include <concrete-callbacks/su_writer.h>
#include <concrete-callbacks/binary_writer.h>
#include <iostream>

using namespace std;

void parse_image_callback(ConfigMap map, CallbackCollection *collection,
                          std::string write_path);
void parse_csv_callback(ConfigMap map, CallbackCollection *collection,
                        std::string write_path);
void parse_norm_callback(ConfigMap map, CallbackCollection *collection,
                         std::string write_path);
void parse_segy_callback(ConfigMap map, CallbackCollection *collection,
                         std::string write_path);
void parse_su_callback(ConfigMap map, CallbackCollection *collection,
                       std::string write_path);
void parse_binary_callback(ConfigMap map, CallbackCollection *collection,
                           std::string write_path);

CallbackCollection *parse_callbacks(std::string filename,
                                    std::string write_path) {
  ConfigMap conf = build_map(filename);
  CallbackCollection *callbacks_collection = new CallbackCollection();
#ifdef USE_OpenCV
  parse_image_callback(conf, callbacks_collection, write_path);
#endif
  parse_csv_callback(conf, callbacks_collection, write_path);
  parse_norm_callback(conf, callbacks_collection, write_path);
  parse_segy_callback(conf, callbacks_collection, write_path);
  parse_su_callback(conf, callbacks_collection, write_path);
  parse_binary_callback(conf, callbacks_collection, write_path);

  return callbacks_collection;
}

void parse_image_callback(ConfigMap map, CallbackCollection *collection,
                          std::string write_path) {
  if (map.find("enable-image") != map.end() && map["enable-image"] == "yes") {
    int show_each = 200;
    float percentile = 100;
    bool write_velocity=false,write_forward=false,write_backward=false,write_reverse=false,
    write_migration=false,write_re_extended_velocity=false,write_single_shot_correlation=false,
    write_each_stacked_shot=false,write_traces_raw=false,write_traces_preprocessed=false;
    if (map.find("image.show_each") != map.end()) {
      show_each = stoi(map["image.show_each"]);
    }
    if (map.find("image.percentile") != map.end()) {
      percentile = stof(map["image.percentile"]);
    }
    if (map.find("write_velocity") != map.end() && map["write_velocity"] == "yes") {
        write_velocity = true;
    }
    if (map.find("write_forward") != map.end() && map["write_forward"] == "yes") {
          write_forward = true;
    }
    if (map.find("write_backward") != map.end() && map["write_backward"] == "yes") {
            write_backward = true;
    }
    if (map.find("write_reverse") != map.end() && map["write_reverse"] == "yes") {
        write_reverse = true;
    }
    if (map.find("write_migration") != map.end() && map["write_migration"] == "yes") {
        write_migration = true;
    }
    if (map.find("write_re_extended_velocity") != map.end() && map["write_re_extended_velocity"] == "yes") {
        write_re_extended_velocity = true;
    }
    if (map.find("write_single_shot_correlation") != map.end() && map["write_single_shot_correlation"] == "yes") {
        write_single_shot_correlation = true;
    }
    if (map.find("write_each_stacked_shot") != map.end() && map["write_each_stacked_shot"] == "yes") {
        write_each_stacked_shot = true;
    }
    if (map.find("write_traces_raw") != map.end() && map["write_traces_raw"] == "yes") {
        write_traces_raw = true;
    }
    if (map.find("write_traces_preprocessed") != map.end() && map["write_traces_preprocessed"] == "yes") {
        write_traces_preprocessed = true;
    }
    auto *image_writer =
        new ImageWriter(show_each,write_velocity, write_forward, write_backward, write_reverse,write_migration , write_re_extended_velocity,write_single_shot_correlation,
                        write_each_stacked_shot , write_traces_raw, write_traces_preprocessed, percentile, write_path);
    collection->RegisterCallback(image_writer);
    cout << "Creating image callback with show_each = " << show_each
         << " and percentile = " << percentile << endl;
  }
}

void parse_csv_callback(ConfigMap map, CallbackCollection *collection,
                        std::string write_path) {
    bool write_velocity=false,write_forward=false,write_backward=false,write_reverse=false,
            write_migration=false,write_re_extended_velocity=false,write_single_shot_correlation=false,
            write_each_stacked_shot=false,write_traces_raw=false,write_traces_preprocessed=false;
  if (map.find("enable-csv") != map.end() && map["enable-csv"] == "yes") {
    int show_each = 200;
    if (map.find("csv.show_each") != map.end()) {
      show_each = stoi(map["csv.show_each"]);
    }
      if (map.find("write_velocity") != map.end() && map["write_velocity"] == "yes") {
          write_velocity = true;
      }
      if (map.find("write_forward") != map.end() && map["write_forward"] == "yes") {
          write_forward = true;
      }
      if (map.find("write_backward") != map.end() && map["write_backward"] == "yes") {
          write_backward = true;
      }
      if (map.find("write_reverse") != map.end() && map["write_reverse"] == "yes") {
          write_reverse = true;
      }
      if (map.find("write_migration") != map.end() && map["write_migration"] == "yes") {
          write_migration = true;
      }
      if (map.find("write_re_extended_velocity") != map.end() && map["write_re_extended_velocity"] == "yes") {
          write_re_extended_velocity = true;
      }
      if (map.find("write_single_shot_correlation") != map.end() && map["write_single_shot_correlation"] == "yes") {
          write_single_shot_correlation = true;
      }
      if (map.find("write_each_stacked_shot") != map.end() && map["write_each_stacked_shot"] == "yes") {
          write_each_stacked_shot = true;
      }
      if (map.find("write_traces_raw") != map.end() && map["write_traces_raw"] == "yes") {
          write_traces_raw = true;
      }
      if (map.find("write_traces_preprocessed") != map.end() && map["write_traces_preprocessed"] == "yes") {
          write_traces_preprocessed = true;
      }
    auto *csv_writer = new CsvWriter(show_each,write_velocity, write_forward, write_backward, write_reverse,write_migration , write_re_extended_velocity,write_single_shot_correlation,
      write_each_stacked_shot , write_traces_raw, write_traces_preprocessed, write_path);
    collection->RegisterCallback(csv_writer);
    cout << "Creating csv callback with show_each = " << show_each << endl;
  }
}

void parse_norm_callback(ConfigMap map, CallbackCollection *collection,
                         std::string write_path) {
  if (map.find("enable-norm") != map.end() && map["enable-norm"] == "yes") {
    int show_each = 200;
    if (map.find("norm.show_each") != map.end()) {
      show_each = stoi(map["norm.show_each"]);
    }
    auto *norm_writer = new NormWriter(show_each, true, true, true, write_path);
    collection->RegisterCallback(norm_writer);
    cout << "Creating norm callback with show_each = " << show_each << endl;
  }
}

void parse_segy_callback(ConfigMap map, CallbackCollection *collection,
                         std::string write_path) {
  if (map.find("enable-segy") != map.end() && map["enable-segy"] == "yes") {
    int show_each = 200;
    bool write_velocity=false,write_forward=false,write_backward=false,write_reverse=false,
              write_migration=false,write_re_extended_velocity=false,write_single_shot_correlation=false,
              write_each_stacked_shot=false,write_traces_raw=false,write_traces_preprocessed=false;
    if (map.find("segy.show_each") != map.end()) {
      show_each = stoi(map["segy.show_each"]);
    }
      if (map.find("write_velocity") != map.end() && map["write_velocity"] == "yes") {
          write_velocity = true;
      }
      if (map.find("write_forward") != map.end() && map["write_forward"] == "yes") {
          write_forward = true;
      }
      if (map.find("write_backward") != map.end() && map["write_backward"] == "yes") {
          write_backward = true;
      }
      if (map.find("write_reverse") != map.end() && map["write_reverse"] == "yes") {
          write_reverse = true;
      }
      if (map.find("write_migration") != map.end() && map["write_migration"] == "yes") {
          write_migration = true;
      }
      if (map.find("write_re_extended_velocity") != map.end() && map["write_re_extended_velocity"] == "yes") {
          write_re_extended_velocity = true;
      }
      if (map.find("write_single_shot_correlation") != map.end() && map["write_single_shot_correlation"] == "yes") {
          write_single_shot_correlation = true;
      }
      if (map.find("write_each_stacked_shot") != map.end() && map["write_each_stacked_shot"] == "yes") {
          write_each_stacked_shot = true;
      }
      if (map.find("write_traces_raw") != map.end() && map["write_traces_raw"] == "yes") {
          write_traces_raw = true;
      }
      if (map.find("write_traces_preprocessed") != map.end() && map["write_traces_preprocessed"] == "yes") {
          write_traces_preprocessed = true;
      }
    auto *segy_writer =
        new SegyWriter(show_each,write_velocity, write_forward, write_backward, write_reverse,write_migration , write_re_extended_velocity,write_single_shot_correlation,
      write_each_stacked_shot , write_traces_raw, write_traces_preprocessed, write_path);;
    collection->RegisterCallback(segy_writer);
    cout << "Creating segy callback with show_each = " << show_each << endl;
  }
}

void parse_su_callback(ConfigMap map, CallbackCollection *collection,
                       std::string write_path) {
    if (map.find("enable-su") != map.end() && map["enable-su"] == "yes") {
        int show_each = 200;
        bool write_little_endian = false;
        bool write_velocity=false,write_forward=false,write_backward=false,write_reverse=false,
                write_migration=false,write_re_extended_velocity=false,write_single_shot_correlation=false,
                write_each_stacked_shot=false,write_traces_raw=false,write_traces_preprocessed=false;
        if (map.find("su.show_each") != map.end()) {
            show_each = stoi(map["su.show_each"]);
        }
        if (map.find("su.write_in_little_endian") != map.end() && map["su.write_in_little_endian"] == "yes") {
            write_little_endian = true;
        }
        if (map.find("write_velocity") != map.end() && map["write_velocity"] == "yes") {
            write_velocity = true;
        }
        if (map.find("write_forward") != map.end() && map["write_forward"] == "yes") {
            write_forward = true;
        }
        if (map.find("write_backward") != map.end() && map["write_backward"] == "yes") {
            write_backward = true;
        }
        if (map.find("write_reverse") != map.end() && map["write_reverse"] == "yes") {
            write_reverse = true;
        }
        if (map.find("write_migration") != map.end() && map["write_migration"] == "yes") {
            write_migration = true;
        }
        if (map.find("write_re_extended_velocity") != map.end() && map["write_re_extended_velocity"] == "yes") {
            write_re_extended_velocity = true;
        }
        if (map.find("write_single_shot_correlation") != map.end() && map["write_single_shot_correlation"] == "yes") {
            write_single_shot_correlation = true;
        }
        if (map.find("write_each_stacked_shot") != map.end() && map["write_each_stacked_shot"] == "yes") {
            write_each_stacked_shot = true;
        }
        if (map.find("write_traces_raw") != map.end() && map["write_traces_raw"] == "yes") {
            write_traces_raw = true;
        }
        if (map.find("write_traces_preprocessed") != map.end() && map["write_traces_preprocessed"] == "yes") {
            write_traces_preprocessed = true;
        }
        auto *su_writer =
                new SuWriter(show_each,write_velocity, write_forward, write_backward, write_reverse,write_migration , write_re_extended_velocity,write_single_shot_correlation,
                             write_each_stacked_shot , write_traces_raw, write_traces_preprocessed, write_path,
                             write_little_endian);
        collection->RegisterCallback(su_writer);
        if (write_little_endian) {
            cout << "Creating SU callback in little endian format with show_each = " << show_each << endl;
        } else {
            cout << "Creating SU callback in big endian format with show_each = " << show_each << endl;
        }
    }
}

void parse_binary_callback(ConfigMap map, CallbackCollection *collection,
                           std::string write_path) {
    if (map.find("enable-binary") != map.end() && map["enable-binary"] == "yes") {
        int show_each = 200;
        bool write_velocity=false,write_forward=false,write_backward=false,write_reverse=false,
                write_migration=false,write_re_extended_velocity=false,write_single_shot_correlation=false,
                write_each_stacked_shot=false,write_traces_raw=false,write_traces_preprocessed=false;
        if (map.find("binary.show_each") != map.end()) {
            show_each = stoi(map["binary.show_each"]);
        }
        if (map.find("write_velocity") != map.end() && map["write_velocity"] == "yes") {
            write_velocity = true;
        }
        if (map.find("write_forward") != map.end() && map["write_forward"] == "yes") {
            write_forward = true;
        }
        if (map.find("write_backward") != map.end() && map["write_backward"] == "yes") {
            write_backward = true;
        }
        if (map.find("write_reverse") != map.end() && map["write_reverse"] == "yes") {
            write_reverse = true;
        }
        if (map.find("write_migration") != map.end() && map["write_migration"] == "yes") {
            write_migration = true;
        }
        if (map.find("write_re_extended_velocity") != map.end() && map["write_re_extended_velocity"] == "yes") {
            write_re_extended_velocity = true;
        }
        if (map.find("write_single_shot_correlation") != map.end() && map["write_single_shot_correlation"] == "yes") {
            write_single_shot_correlation = true;
        }
        if (map.find("write_each_stacked_shot") != map.end() && map["write_each_stacked_shot"] == "yes") {
            write_each_stacked_shot = true;
        }
        if (map.find("write_traces_raw") != map.end() && map["write_traces_raw"] == "yes") {
            write_traces_raw = true;
        }
        if (map.find("write_traces_preprocessed") != map.end() && map["write_traces_preprocessed"] == "yes") {
            write_traces_preprocessed = true;
        }
        auto *binary_writer =
                new BinaryWriter(show_each,write_velocity, write_forward, write_backward, write_reverse,write_migration , write_re_extended_velocity,write_single_shot_correlation,
                                 write_each_stacked_shot , write_traces_raw, write_traces_preprocessed, write_path);
        collection->RegisterCallback(binary_writer);
        cout << "Creating binary callback with show_each = " << show_each << endl;
    }
}