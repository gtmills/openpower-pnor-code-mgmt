#pragma once

#include <sdbusplus/server.hpp>
#include <xyz/openbmc_project/Software/Activation/server.hpp>
#include <xyz/openbmc_project/Software/ActivationBlocksTransition/server.hpp>
#include "xyz/openbmc_project/Software/ExtendedVersion/server.hpp"
#include "xyz/openbmc_project/Software/RedundancyPriority/server.hpp"
#include "xyz/openbmc_project/Software/ActivationProgress/server.hpp"
#include "xyz/openbmc_project/Object/Delete/server.hpp"
#include "org/openbmc/Associations/server.hpp"

namespace openpower
{
namespace software
{
namespace updater
{

using AssociationList =
        std::vector<std::tuple<std::string, std::string, std::string>>;
using ActivationInherit = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Object::server::Delete,
    sdbusplus::xyz::openbmc_project::Software::server::ExtendedVersion,
    sdbusplus::xyz::openbmc_project::Software::server::Activation,
    sdbusplus::org::openbmc::server::Associations>;
using ActivationBlocksTransitionInherit = sdbusplus::server::object::object<
 sdbusplus::xyz::openbmc_project::Software::server::ActivationBlocksTransition>;
using RedundancyPriorityInherit = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Software::server::RedundancyPriority>;
using ActivationProgressInherit = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Software::server::ActivationProgress>;

namespace sdbusRule = sdbusplus::bus::match::rules;

class ItemUpdater;
class Activation;
class RedundancyPriority;

/** @class RedundancyPriority
 *  @brief OpenBMC RedundancyPriority implementation
 *  @details A concrete implementation for
 *  xyz.openbmc_project.Software.RedundancyPriority DBus API.
 */
class RedundancyPriority : public RedundancyPriorityInherit
{
    public:
        /** @brief Constructs RedundancyPriority.
         *
         *  @param[in] bus    - The Dbus bus object
         *  @param[in] path   - The Dbus object path
         *  @param[in] parent - Parent object.
         *  @param[in] value  - The redundancyPriority value
         */
        RedundancyPriority(sdbusplus::bus::bus& bus,
                                   const std::string& path,
                                   Activation& parent,
                                   uint8_t value) :
                                   RedundancyPriorityInherit(bus,
                                   path.c_str(), true),
                                   parent(parent),
                                   bus(bus),
                                   path(path)
        {
            // Set Property
            priority(value);
            std::vector<std::string> interfaces({interface});
            bus.emit_interfaces_added(path.c_str(), interfaces);
        }

        ~RedundancyPriority()
        {
            std::vector<std::string> interfaces({interface});
            bus.emit_interfaces_removed(path.c_str(), interfaces);
        }

        /** @brief Overloaded Priority property set function
         *
         *  @param[in] value - uint8_t
         *
         *  @return Success or exception thrown
         */
        uint8_t priority(uint8_t value) override;

        /** @brief Priority property get function
         *
         * @returns uint8_t - The Priority value
         */
        using RedundancyPriorityInherit::priority;

        /** @brief Parent Object. */
        Activation& parent;

    private:
        // TODO Remove once openbmc/openbmc#1975 is resolved
        static constexpr auto interface =
                "xyz.openbmc_project.Software.RedundancyPriority";
        sdbusplus::bus::bus& bus;
        std::string path;
};

/** @class ActivationBlocksTransition
 *  @brief OpenBMC ActivationBlocksTransition implementation.
 *  @details A concrete implementation for
 *  xyz.openbmc_project.Software.ActivationBlocksTransition DBus API.
 */
class ActivationBlocksTransition : public ActivationBlocksTransitionInherit
{
    public:
        /** @brief Constructs ActivationBlocksTransition.
         *
         * @param[in] bus    - The Dbus bus object
         * @param[in] path   - The Dbus object path
         */
        ActivationBlocksTransition(sdbusplus::bus::bus& bus,
                                   const std::string& path) :
                   ActivationBlocksTransitionInherit(bus, path.c_str(), true),
                   bus(bus),
                   path(path)
        {
            std::vector<std::string> interfaces({interface});
            bus.emit_interfaces_added(path.c_str(), interfaces);
        }

        ~ActivationBlocksTransition()
        {
            std::vector<std::string> interfaces({interface});
            bus.emit_interfaces_removed(path.c_str(), interfaces);
        }

    private:
        // TODO Remove once openbmc/openbmc#1975 is resolved
        static constexpr auto interface =
                "xyz.openbmc_project.Software.ActivationBlocksTransition";
        sdbusplus::bus::bus& bus;
        std::string path;
};

class ActivationProgress : public ActivationProgressInherit
{
    public:
        /** @brief Constructs ActivationProgress.
         *
         * @param[in] bus    - The Dbus bus object
         * @param[in] path   - The Dbus object path
         */
        ActivationProgress(sdbusplus::bus::bus& bus,
                           const std::string& path) :
                   ActivationProgressInherit(bus, path.c_str(), true),
                   bus(bus),
                   path(path)
       {
           progress(0);
           std::vector<std::string> interfaces({interface});
           bus.emit_interfaces_added(path.c_str(), interfaces);
       }

        ~ActivationProgress()
        {
            std::vector<std::string> interfaces({interface});
            bus.emit_interfaces_removed(path.c_str(), interfaces);
        }

    private:
        // TODO Remove once openbmc/openbmc#1975 is resolved
        static constexpr auto interface =
                "xyz.openbmc_project.Software.ActivationProgress";
        sdbusplus::bus::bus& bus;
        std::string path;
};

/** @class Activation
 *  @brief OpenBMC activation software management implementation.
 *  @details A concrete implementation for
 *  xyz.openbmc_project.Software.Activation DBus API.
 */
class Activation : public ActivationInherit
{
    public:
        /** @brief Constructs Activation Software Manager
         *
         * @param[in] bus    - The Dbus bus object
         * @param[in] path   - The Dbus object path
         * @param[in] parent - Parent object.
         * @param[in] versionId  - The software version id
         * @param[in] extVersion - The extended version
         * @param[in] activationStatus - The status of Activation
         * @param[in] assocs - Association objects
         */
        Activation(sdbusplus::bus::bus& bus, const std::string& path,
                   ItemUpdater& parent,
                   std::string& versionId,
                   std::string& extVersion,
                   sdbusplus::xyz::openbmc_project::Software::
                   server::Activation::Activations activationStatus,
                   AssociationList& assocs) :
                   ActivationInherit(bus, path.c_str(), true),
                   bus(bus),
                   path(path),
                   parent(parent),
                   versionId(versionId),
                   systemdSignals(
                           bus,
                           sdbusRule::type::signal() +
                           sdbusRule::member("JobRemoved") +
                           sdbusRule::path("/org/freedesktop/systemd1") +
                           sdbusRule::interface(
                                   "org.freedesktop.systemd1.Manager"),
                           std::bind(std::mem_fn(&Activation::unitStateChange),
                                  this, std::placeholders::_1))
        {
            // Enable systemd signals
            subscribeToSystemdSignals();
            // Set Properties.
            extendedVersion(extVersion);
            activation(activationStatus);
            associations(assocs);

            // Emit deferred signal.
            emit_object_added();
        }

        /** @brief Activation property get function
         *
         *  @returns One of Activation::Activations
         */
        using ActivationInherit::activation;

        /** @brief Overloaded Activation property setter function
         *
         *  @param[in] value - One of Activation::Activations
         *
         *  @return Success or exception thrown
         */
        Activations activation(Activations value) override;

        /** @brief Overloaded requestedActivation property setter function
         *
         *  @param[in] value - One of Activation::RequestedActivations
         *
         *  @return Success or exception thrown
         */
        RequestedActivations requestedActivation(RequestedActivations value)
                override;

        /** @brief Check if systemd state change is relevant to this object
         *
         * Instance specific interface to handle the detected systemd state
         * change
         *
         * @param[in]  msg       - Data associated with subscribed signal
         *
         */
        void unitStateChange(sdbusplus::message::message& msg);

        /**
         * @brief subscribe to the systemd signals
         *
         * This object needs to capture when it's systemd targets complete
         * so it can keep it's state updated
         *
         **/
        void subscribeToSystemdSignals();

        /**
         * @brief unsubscribe from the systemd signals
         *
         * Once the activation process has completed successfully, we can
         * safely unsubscribe from systemd signals.
         *
         **/
        void unsubscribeFromSystemdSignals();

        /** @brief Persistent sdbusplus DBus bus connection */
        sdbusplus::bus::bus& bus;

        /** @brief Persistent DBus object path */
        std::string path;

        /** @brief Parent Object. */
        ItemUpdater& parent;

        /** @brief Version id */
        std::string versionId;

        /** @brief Persistent ActivationBlocksTransition dbus object */
        std::unique_ptr<ActivationBlocksTransition> activationBlocksTransition;

        /** @brief Persistent ActivationProgress dbus object */
        std::unique_ptr<ActivationProgress> activationProgress;

        /** @brief Persistent RedundancyPriority dbus object */
        std::unique_ptr<RedundancyPriority> redundancyPriority;

        /** @brief Used to subscribe to dbus systemd signals **/
        sdbusplus::bus::match_t systemdSignals;

        /** @brief Tracks whether the read-only & read-write volumes have been
         *created as part of the activation process. **/
        bool ubiVolumesCreated = false;

        /** @brief activation status property get function
         *
         * @returns Activations - The activation value
         */
        using ActivationInherit::activation;

        /** @brief Deletes the d-bus object.
         *
         *
         * */
        void delete_() override;

    private:
        /** @brief Member function for clarity & brevity at activation start */
        void startActivation();

        /** @brief Member function for clarity & brevity at activation end */
        void finishActivation();
};

} // namespace updater
} // namespace software
} // namespace openpower
