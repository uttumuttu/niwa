/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACER_PHOTONTRACER_H
#define NIWA_RAYTRACER_PHOTONTRACER_H

#include <boost/shared_ptr.hpp>
#include <memory>

namespace niwa {
    namespace system {
        class IParallelizer;
    }

    namespace photonmap {
        class IPhotonMap;
    }

    namespace random {
        class RandomSet;
    }

    namespace raytrace {
        class ITraceable;
        class ILight;

        class PhotonTracer {
        public:
            PhotonTracer();
            ~PhotonTracer();

            void setScene(boost::shared_ptr<ITraceable> scene);

            void setLight(boost::shared_ptr<ILight> light);

            void setParallelizer(
                boost::shared_ptr<system::IParallelizer> parallelizer);

            void tracePhotons(
                photonmap::IPhotonMap& photonMap, int photonCount) const;

        private:
            void traceSinglePhoton(
                photonmap::IPhotonMap& photonMap, int photonCount) const;

        private:
            class TraceTask;

        private: // prevent copying
            PhotonTracer(PhotonTracer const&);
            PhotonTracer& operator = (PhotonTracer const&);

        private:
            boost::shared_ptr<ITraceable> scene_;

            boost::shared_ptr<ILight> light_;

            boost::shared_ptr<system::IParallelizer> parallelizer_;

            mutable std::auto_ptr<random::RandomSet> randomSet_;
        };
    }
}

#endif