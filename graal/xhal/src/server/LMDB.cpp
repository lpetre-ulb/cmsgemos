#include "xhal/server/LMDB.h"

#include <limits>
#include <memory>

#define PATH_VAR "GEM_PATH"
#define DB_NAME "/address_table.mdb"

namespace /* anonymous */ {
  /// @brief Maximum size of the LMDB object, currently 50 MiB;
  static const std::size_t MAP_SIZE = 50UL * 1024UL * 1024UL;

  /**
   * @brief Creates the environment.
   *
   * Required as a separate function for the Singleton constructor.
   */
  lmdb::env create_env()
  {
    auto env = lmdb::env::create();
    env.set_mapsize(MAP_SIZE);

    const char * path = std::getenv(PATH_VAR);
    if (path == nullptr) {
      throw std::runtime_error("Environment variable " PATH_VAR " is not defined");
    }
    std::string fullPath = path;
    fullPath += DB_NAME;;
    env.open(fullPath.c_str(), 0, 0664);

    return env;
  }

  /**
   * @brief Shared data managed by the guards.
   */
  struct Singleton
  {
    // NOTE: Order is important!
    lmdb::env env;  ///< @brief Environment
    lmdb::txn rtxn; ///< @brief Read-only transaction
    lmdb::dbi dbi;  ///< @brief Database handle

    /**
     * @brief Constructor.
     *
     * A constructor is required because LMDB objects don't have default constructors.
     */
    Singleton() : // NOTE: Order is important!
      env(create_env()),
      rtxn(lmdb::txn::begin(env, nullptr, MDB_RDONLY)),
      dbi(lmdb::dbi::open(rtxn, nullptr))
    {
    }
  };

  /**
   * @brief Points to the data managed by the guards.
   */
  std::unique_ptr<Singleton> SINGLETON = nullptr;

  /**
   * @brief The number of guards currently active.
   */
  int GUARD_COUNT = 0;
} // anonymous namespace

xhal::server::LMDBGuard::LMDBGuard()
{
  if (GUARD_COUNT <= 0 || SINGLETON == nullptr) {
    SINGLETON = std::unique_ptr<Singleton>(); // Initialize
    GUARD_COUNT = 1;
  } else if (GUARD_COUNT == std::numeric_limits<int>::max()) {
    throw std::runtime_error("Out of LMDB guard handles");
  } else {
    ++GUARD_COUNT; // Add a reference
  }
}

xhal::server::LMDBGuard::~LMDBGuard() noexcept
{
  --GUARD_COUNT;
  if (GUARD_COUNT <= 0) {
    // Free shared resources
    SINGLETON = nullptr;
  }
}

lmdb::env &xhal::server::LMDBGuard::env() noexcept
{
  return SINGLETON->env;
}

const lmdb::env &xhal::server::LMDBGuard::env() const noexcept
{
  return SINGLETON->env;
}

lmdb::dbi &xhal::server::LMDBGuard::dbi() noexcept
{
  return SINGLETON->dbi;
}

const lmdb::dbi &xhal::server::LMDBGuard::dbi() const noexcept
{
  return SINGLETON->dbi;
}

lmdb::txn &xhal::server::LMDBGuard::rtxn() noexcept
{
  return SINGLETON->rtxn;
}

const lmdb::txn &xhal::server::LMDBGuard::rtxn() const noexcept
{
  return SINGLETON->rtxn;
}
