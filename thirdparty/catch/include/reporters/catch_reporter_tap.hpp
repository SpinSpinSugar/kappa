/*
 *  Created by Colton Wolkins on 2015-08-15.
 *  Copyright 2015 Martin Moene. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_TAP_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_TAP_HPP_INCLUDED

// Don't #include any Catch headers here - we can assume they are already
// included before this header.
// This is not good practice in general but is necessary in this case so this
// file can be distributed as a single header that works with the main
// Catch single header.

#include <algorithm>

namespace Catch {

struct TAPReporter : StreamingReporterBase {
  TAPReporter(ReporterConfig const &_config)
      : StreamingReporterBase(_config), counter(0) {}

  virtual ~TAPReporter();

  static std::string getDescription() {
    return "Reports test results in TAP format, suitable for test harneses";
  }

  virtual ReporterPreferences getPreferences() const {
    ReporterPreferences prefs;
    prefs.shouldRedirectStdOut = false;
    return prefs;
  }

  virtual void noMatchingTestCases(std::string const &spec) {
    stream << "# No test cases matched '" << spec << "'" << std::endl;
  }

  virtual void assertionStarting(AssertionInfo const &) {}

  virtual bool assertionEnded(AssertionStats const &_assertionStats) {
    ++counter;

    AssertionPrinter printer(stream, _assertionStats, counter);
    printer.print();
    stream << " # " << currentTestCaseInfo->name;

    stream << std::endl;
    return true;
  }

  virtual void testRunEnded(TestRunStats const &_testRunStats) {
    printTotals(_testRunStats.totals);
    stream << "\n" << std::endl;
    StreamingReporterBase::testRunEnded(_testRunStats);
  }

 private:
  size_t counter;
  class AssertionPrinter {
    void operator=(AssertionPrinter const &);

   public:
    AssertionPrinter(std::ostream &_stream, AssertionStats const &_stats,
                     size_t counter)
        : stream(_stream),
          stats(_stats),
          result(_stats.assertionResult),
          messages(_stats.infoMessages),
          itMessage(_stats.infoMessages.begin()),
          printInfoMessages(true),
          counter(counter) {}

    void print() {
      itMessage = messages.begin();

      switch (result.getResultType()) {
        case ResultWas::Ok:
          printResultType(passedString());
          printOriginalExpression();
          printReconstructedExpression();
          if (!result.hasExpression())
            printRemainingMessages(Colour::None);
          else
            printRemainingMessages();
          break;
        case ResultWas::ExpressionFailed:
          if (result.isOk()) {
            printResultType(passedString());
          } else {
            printResultType(failedString());
          }
          printOriginalExpression();
          printReconstructedExpression();
          if (result.isOk()) {
            printIssue(" # TODO");
          }
          printRemainingMessages();
          break;
        case ResultWas::ThrewException:
          printResultType(failedString());
          printIssue("unexpected exception with message:");
          printMessage();
          printExpressionWas();
          printRemainingMessages();
          break;
        case ResultWas::FatalErrorCondition:
          printResultType(failedString());
          printIssue("fatal error condition with message:");
          printMessage();
          printExpressionWas();
          printRemainingMessages();
          break;
        case ResultWas::DidntThrowException:
          printResultType(failedString());
          printIssue("expected exception, got none");
          printExpressionWas();
          printRemainingMessages();
          break;
        case ResultWas::Info:
          printResultType("info");
          printMessage();
          printRemainingMessages();
          break;
        case ResultWas::Warning:
          printResultType("warning");
          printMessage();
          printRemainingMessages();
          break;
        case ResultWas::ExplicitFailure:
          printResultType(failedString());
          printIssue("explicitly");
          printRemainingMessages(Colour::None);
          break;
        // These cases are here to prevent compiler warnings
        case ResultWas::Unknown:
        case ResultWas::FailureBit:
        case ResultWas::Exception:
          printResultType("** internal error **");
          break;
      }
    }

   private:
    static Colour::Code dimColour() { return Colour::FileName; }

    static const char *failedString() { return "not ok"; }
    static const char *passedString() { return "ok"; }

    void printSourceInfo() const {
      Colour colourGuard(dimColour());
      stream << result.getSourceInfo() << ":";
    }

    void printResultType(std::string const &passOrFail) const {
      if (!passOrFail.empty()) {
        stream << passOrFail << ' ' << counter << " -";
      }
    }

    void printIssue(std::string const &issue) const { stream << " " << issue; }

    void printExpressionWas() {
      if (result.hasExpression()) {
        stream << ";";
        {
          Colour colour(dimColour());
          stream << " expression was:";
        }
        printOriginalExpression();
      }
    }

    void printOriginalExpression() const {
      if (result.hasExpression()) {
        stream << " " << result.getExpression();
      }
    }

    void printReconstructedExpression() const {
      if (result.hasExpandedExpression()) {
        {
          Colour colour(dimColour());
          stream << " for: ";
        }
        std::string expr = result.getExpandedExpression();
        std::replace(expr.begin(), expr.end(), '\n', ' ');
        stream << expr;
      }
    }

    void printMessage() {
      if (itMessage != messages.end()) {
        stream << " '" << itMessage->message << "'";
        ++itMessage;
      }
    }

    void printRemainingMessages(Colour::Code colour = dimColour()) {
      if (itMessage == messages.end()) {
        return;
      }

      // using messages.end() directly yields compilation error:
      std::vector<MessageInfo>::const_iterator itEnd = messages.end();
      const std::size_t N =
          static_cast<std::size_t>(std::distance(itMessage, itEnd));

      {
        Colour colourGuard(colour);
        stream << " with " << pluralise(N, "message") << ":";
      }

      for (; itMessage != itEnd;) {
        // If this assertion is a warning ignore any INFO messages
        if (printInfoMessages || itMessage->type != ResultWas::Info) {
          stream << " '" << itMessage->message << "'";
          if (++itMessage != itEnd) {
            Colour colourGuard(dimColour());
            stream << " and";
          }
        }
      }
    }

   private:
    std::ostream &stream;
    AssertionStats const &stats;
    AssertionResult const &result;
    std::vector<MessageInfo> messages;
    std::vector<MessageInfo>::const_iterator itMessage;
    bool printInfoMessages;
    size_t counter;
  };

  void printTotals(const Totals &totals) const {
    if (totals.testCases.total() == 0) {
      stream << "1..0 # Skipped: No tests ran.";
    } else {
      stream << "1.." << counter;
    }
  }
};

#ifdef CATCH_IMPL
TAPReporter::~TAPReporter() {}
#endif

INTERNAL_CATCH_REGISTER_REPORTER("tap", TAPReporter)

}  // end namespace Catch

#endif  // TWOBLUECUBES_CATCH_REPORTER_TAP_HPP_INCLUDED
