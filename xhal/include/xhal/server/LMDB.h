/**
 * @file
 * @brief This file contains utilities dealing with the LMDB register database.
 *
 * @author Louis Moureaux <lmoureau@ulb.ac.be>
 */

#ifndef XHAL_SERVER_LMDB_H
#define XHAL_SERVER_LMDB_H

#include <lmdb++.h>

namespace xhal {
  namespace server {

    /**
     * @brief Provides access to shared LMDB data structures.
     *
     * This class uses the "guard" pattern to provide access to LMDB data structures: an
     * environment, a database handle and a read-only transaction. These objects are
     * guaranteed to be accessible for the lifetime of the guard.
     *
     * This guard is recursive: several instances of it can safely be nested. The recommended
     * usage patterns are as follows:
     *
     *  * If the guard should be kept alive for the lifetime of an object, use private
     *    inheritance on the corresponding class.
     *  * If the guard is only required within a function, declare an instance of it.
     *
     * When the first guard is created, it sets up the objects required to read from the
     * database. These objects are released automatically when the last guard is deleted.
     *
     * @warning This class is not thread-safe.
     */
    class LMDBGuard
    {
      public:
        /**
         * @brief Constructs a guard.
         */
        LMDBGuard();

        /**
         * @brief Copy constructor.
         */
        LMDBGuard(const LMDBGuard &) = default;

        /**
         * @brief Assignment operator.
         */
        LMDBGuard &operator=(const LMDBGuard &) = default;

        /**
         * @brief Move constructor.
         */
        constexpr LMDBGuard(LMDBGuard &&) = default;

        /**
         * @brief Move operator.
         */
        // Can't be made constexpr in GCC 4.9
        LMDBGuard &operator=(LMDBGuard &&) = default;

        /**
         * @brief Destructor. Resources are freed when the last guard is deleted.
         */
        ~LMDBGuard() noexcept;

        /**
         * @brief Retrieves the LMDB environment.
         */
        lmdb::env &env() noexcept;

        /**
         * @brief Retrieves the LMDB environment (@c const version).
         */
        const lmdb::env &env() const noexcept;

        /**
         * @brief Retrieves the LMDB database handle.
         */
        lmdb::dbi &dbi() noexcept;

        /**
         * @brief Retrieves the LMDB database handle (@c const version).
         */
        const lmdb::dbi &dbi() const noexcept;

        /**
         * @brief Retrieves a read-only LMDB transaction.
         */
        lmdb::txn &rtxn() noexcept;

        /**
         * @brief Retrieves a read-only LMDB transaction (@c const version).
         */
        const lmdb::txn &rtxn() const noexcept;
    };

  } // namespace xhal::server
} // namespace xhal

#endif // XHAL_SERVER_LMDBXS_H
