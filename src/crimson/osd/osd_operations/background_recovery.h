// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#pragma once

#include "crimson/net/Connection.h"
#include "crimson/osd/osd_operation.h"
#include "crimson/common/type_helpers.h"

#include "messages/MOSDOp.h"

namespace crimson::osd {
class PG;
class ShardServices;

class BackgroundRecovery : public OperationT<BackgroundRecovery> {
public:
  static constexpr OperationTypeCode type = OperationTypeCode::background_recovery;

  BackgroundRecovery(
    Ref<PG> pg,
    ShardServices &ss,
    epoch_t epoch_started,
    crimson::osd::scheduler::scheduler_class_t scheduler_class);

  virtual void print(std::ostream &) const;
  virtual void dump_detail(Formatter *f) const;
  seastar::future<> start();
protected:
  Ref<PG> pg;
  ShardServices &ss;
  epoch_t epoch_started;
  crimson::osd::scheduler::scheduler_class_t scheduler_class;
  auto get_scheduler_params(crimson::osd::scheduler::cost_t cost = 1,
			    crimson::osd::scheduler::client_t owner = 0) const {
    return crimson::osd::scheduler::params_t{
      cost, // cost
      owner, // owner
      scheduler_class
    };
  }
  virtual seastar::future<bool> do_recovery();
};

class UrgentRecovery final : public BackgroundRecovery {
public:
  UrgentRecovery(
    const hobject_t& soid,
    const eversion_t& need,
    Ref<PG> pg,
    ShardServices& ss,
    epoch_t epoch_started,
    crimson::osd::scheduler::scheduler_class_t scheduler_class)
  : BackgroundRecovery{pg, ss, epoch_started, scheduler_class},
    soid{soid}, need(need) {}
  void print(std::ostream&) const final;
  void dump_detail(Formatter* f) const final;
private:
  const hobject_t soid;
  const eversion_t need;
  seastar::future<bool> do_recovery() override;
};

}
