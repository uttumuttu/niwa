/**
 * @namespace niwa::logging
 *
 * A thread-safe logging framework similar to Apache's log4j.
 *
 * <h2>How to Use Loggers?</h2>
 *
 * The use of Logger objects is documented well
 * in its own class.
 *
 * <h2>How to Setup Logging in an Application?</h2>
 *
 * The idiomatic way of setting logging is as follows.
 *
 * <p><code>
 * // main.cpp<br />
 * \#include "niwa/logging/Logger.h"<br />
 * \#include "niwa/logging/Level.h"<br />
 * \#include "niwa/logging/ScopedAppender.h"<br />
 * \#include "niwa/logging/appenders/StreamAppender.h"<br />
 * <br />
 * using namespace niwa::logging;<br />
 * using namespace niwa::logging::appenders;<br />
 * <br />
 * int main() {<br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// Choose global log level (can be changed anytime).</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;Logger::setLevel(Level::warn());<br />
 * <br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// Create a stream appender to std::cout.</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// (Notice we could as well use a file stream.)</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;StreamAppender appender(std::cout);<br />
 * <br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// ScopedAppender is a RAII class that adds</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// a given appender to the logging system</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// for the lifetime of the ScopedAppender.</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;ScopedAppender scopedAppender(appender);<br />
 * <br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// HERE GOES APPLICATION</i><br />
 * <br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// At this point, ScopedAppender goes</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// out of scope and thus our StreamAppender</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp;<i>// is removed from the logging system.</i><br />
 * &nbsp;&nbsp;&nbsp;&nbsp; return 0;<br />
 * }
 * </code></p>
 */