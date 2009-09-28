#include <iostream>
#include <cstring>
#include <string>
#include "glite/wms/common/utilities/jobdir.h"
#include "glite/wms/common/utilities/jobdir_reader.h"
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/filelist_reader.h"

using namespace glite::wms::common::utilities;

bool fl2jd(std::string const& source, std::string const& destination, bool keep);
bool jd2fl(std::string const& source, std::string const& destination, bool keep);

void usage(std::ostream& os, std::string const& program_name)
{
  os << "Usage: " << basename(program_name.c_str()) << " [-k|--keep] SOURCE DESTINATION\n"
     << "Convert SOURCE from FileList (JobDir) format to JobDir (FileList), saving the result in DESTINATION\n\n"
     << "  -k, --keep    do not delete the items from SOURCE\n";
}

int main(int argc, char* argv[])
{
  bool keep = false;
  std::string source;
  std::string destination;

  if (argc == 1) {
    usage(std::cout, argv[0]);
    return EXIT_SUCCESS;
  } else {
    if (!strcmp(argv[1], "--keep") || !strcmp(argv[1], "-k")) {
      if (argc != 4) {
        usage(std::cerr, argv[0]);
        return EXIT_FAILURE;
      }
      keep = true;
      source = argv[2];
      destination = argv[3];
    } else if (argc == 3) {
      source = argv[1];
      destination = argv[2];
    } else {
      usage(std::cerr, argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (!fl2jd(source, destination, keep) && !jd2fl(source, destination, keep)) {
    std::cerr << "both fl->jd and jd->fl failed\n";
    return EXIT_FAILURE;
  }
}

bool to_jd(InputReader& src, JobDir& jd, bool keep)
{
  InputReader::InputItems items(src.read());
  InputReader::InputItems::iterator first = items.begin();
  InputReader::InputItems::iterator const last = items.end();
  for ( ; first != last; ++first) {
    jd.deliver((*first)->value());
    if (!keep) {
      (*first)->remove_from_input();
    }
  }

  return true;
}

bool to_fl(InputReader& src, FileList<std::string>& fl, bool keep)
{
  InputReader::InputItems items(src.read());
  InputReader::InputItems::iterator first = items.begin();
  InputReader::InputItems::iterator const last = items.end();
  for ( ; first != last; ++first) {
    fl.push_back((*first)->value());
    if (!keep) {
      (*first)->remove_from_input();
    }
  }

  return true;
}

bool fl2jd(std::string const& source, std::string const& destination, bool keep)
{
  try {
    FileListReader fl(source);
    JobDir jd(destination);
    return to_jd(fl, jd, keep);
  } catch (FileContainerError&) {
    return false;
  } catch (JobDirError&) {
    return false;
  }
  return false;
}

bool jd2fl(std::string const& source, std::string const& destination, bool keep)
{
  try {
    JobDirReader jd(source);
    FileList<std::string> fl(destination);
    return to_fl(jd, fl, keep);
  } catch (JobDirError&) {
    return false;
  } catch (FileContainerError&) {
    return false;
  }
  return false;
}

