/*
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "catch_context.h"
#include "catch_interfaces_config.h"
#include "catch_stream.h"
#include "catch_test_spec_parser.hpp"

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace Catch {

struct ConfigData {
  ConfigData()
      : listTests(false),
        listTags(false),
        listReporters(false),
        listTestNamesOnly(false),
        showSuccessfulTests(false),
        shouldDebugBreak(false),
        noThrow(false),
        showHelp(false),
        showInvisibles(false),
        filenamesAsTags(false),
        abortAfter(-1),
        rngSeed(0),
        verbosity(Verbosity::Normal),
        warnings(WarnAbout::Nothing),
        showDurations(ShowDurations::DefaultForReporter),
        runOrder(RunTests::InDeclarationOrder),
        useColour(UseColour::Auto) {}

  bool listTests;
  bool listTags;
  bool listReporters;
  bool listTestNamesOnly;

  bool showSuccessfulTests;
  bool shouldDebugBreak;
  bool noThrow;
  bool showHelp;
  bool showInvisibles;
  bool filenamesAsTags;

  int abortAfter;
  unsigned int rngSeed;

  Verbosity::Level verbosity;
  WarnAbout::What warnings;
  ShowDurations::OrNot showDurations;
  RunTests::InWhatOrder runOrder;
  UseColour::YesOrNo useColour;

  std::string outputFilename;
  std::string name;
  std::string processName;

  std::vector<std::string> reporterNames;
  std::vector<std::string> testsOrTags;
  std::vector<std::string> sectionsToRun;
};

class Config : public SharedImpl<IConfig> {
 private:
  Config(Config const &other);
  Config &operator=(Config const &other);
  virtual void dummy();

 public:
  Config() {}

  Config(ConfigData const &data) : m_data(data), m_stream(openStream()) {
    if (!data.testsOrTags.empty()) {
      TestSpecParser parser(ITagAliasRegistry::get());
      for (std::size_t i = 0; i < data.testsOrTags.size(); ++i)
        parser.parse(data.testsOrTags[i]);
      m_testSpec = parser.testSpec();
    }
  }

  virtual ~Config() {}

  std::string const &getFilename() const { return m_data.outputFilename; }

  bool listTests() const { return m_data.listTests; }
  bool listTestNamesOnly() const { return m_data.listTestNamesOnly; }
  bool listTags() const { return m_data.listTags; }
  bool listReporters() const { return m_data.listReporters; }

  std::string getProcessName() const { return m_data.processName; }

  std::vector<std::string> const &getReporterNames() const {
    return m_data.reporterNames;
  }
  std::vector<std::string> const &getSectionsToRun() const CATCH_OVERRIDE {
    return m_data.sectionsToRun;
  }

  virtual TestSpec const &testSpec() const CATCH_OVERRIDE { return m_testSpec; }

  bool showHelp() const { return m_data.showHelp; }

  // IConfig interface
  virtual bool allowThrows() const CATCH_OVERRIDE { return !m_data.noThrow; }
  virtual std::ostream &stream() const CATCH_OVERRIDE {
    return m_stream->stream();
  }
  virtual std::string name() const CATCH_OVERRIDE {
    return m_data.name.empty() ? m_data.processName : m_data.name;
  }
  virtual bool includeSuccessfulResults() const CATCH_OVERRIDE {
    return m_data.showSuccessfulTests;
  }
  virtual bool warnAboutMissingAssertions() const CATCH_OVERRIDE {
    return m_data.warnings & WarnAbout::NoAssertions;
  }
  virtual ShowDurations::OrNot showDurations() const CATCH_OVERRIDE {
    return m_data.showDurations;
  }
  virtual RunTests::InWhatOrder runOrder() const CATCH_OVERRIDE {
    return m_data.runOrder;
  }
  virtual unsigned int rngSeed() const CATCH_OVERRIDE { return m_data.rngSeed; }
  virtual UseColour::YesOrNo useColour() const CATCH_OVERRIDE {
    return m_data.useColour;
  }
  virtual bool shouldDebugBreak() const CATCH_OVERRIDE {
    return m_data.shouldDebugBreak;
  }
  virtual int abortAfter() const CATCH_OVERRIDE { return m_data.abortAfter; }
  virtual bool showInvisibles() const CATCH_OVERRIDE {
    return m_data.showInvisibles;
  }

 private:
  IStream const *openStream() {
    if (m_data.outputFilename.empty())
      return new CoutStream();
    else if (m_data.outputFilename[0] == '%') {
      if (m_data.outputFilename == "%debug")
        return new DebugOutStream();
      else
        throw std::domain_error("Unrecognised stream: " +
                                m_data.outputFilename);
    } else
      return new FileStream(m_data.outputFilename);
  }
  ConfigData m_data;

  CATCH_AUTO_PTR(IStream const) m_stream;
  TestSpec m_testSpec;
};

}  // end namespace Catch

#endif  // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
