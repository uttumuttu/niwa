/**
 * @file
 * @author Mikko Kauppila
 *
 * A dependency injection demo similar to the
 * slides of the talk "Big Modular Java with Guice"
 * presented at Google IO 2009.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/inject/Injector.h"
#include "niwa/inject/Module.h"
#include "niwa/inject/Inject.h"

using namespace niwa::inject;

#include <iostream>
#include <memory>

#include <string>                
using std::string;

class IShortener {
public:
    virtual ~IShortener() {}

    virtual string shorten(string text) const = 0;
};

class ITweeter {
public:
    virtual ~ITweeter() {}

    virtual void send(string text) = 0;
};

class BrokenShortener : public IShortener {
public:
    INJECT0(BrokenShortener);

    string shorten(string text) const {
        return text;
    }
};

class ScreenTweeter : public ITweeter {
public:
    INJECT0(ScreenTweeter);

    void send(string text) {
        std::cout << text << std::endl;
    }
};

class TweetClient {
public:
    INJECT2(TweetClient, IShortener, ITweeter);

    TweetClient(IShortener* shortener, ITweeter* tweeter)
        : shortener_(shortener), tweeter_(tweeter) {
        // ignored
    }

    ~TweetClient() {
        delete shortener_;
        delete tweeter_;
    }

    void send(string s) {
        if(s.length() >= 140) {
            s = shortener_->shorten(s);
        }
        tweeter_->send(s);
    }
private:
    IShortener* shortener_; // owned
    ITweeter* tweeter_; // owned
};

class MyModule : public Module {
public:
    void configure() {
        bind<IShortener>().to<BrokenShortener>();

        bind<ITweeter>().to<ScreenTweeter>();

        bind<TweetClient>().to<TweetClient>();
    }
};

int main(int /*argc*/, char** /*argv*/) {
    std::auto_ptr<Injector> injector(Injector::create(new MyModule()));

    std::auto_ptr<TweetClient> client(injector->newInstance<TweetClient>());

    if(client.get()) {
        client->send("Hello, world!");
    } else {
        std::cout << "error: couldn't create client" << std::endl;
    }
    
    return 0;
}