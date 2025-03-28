/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2025,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "manager-base.hpp"

namespace nfd {

ManagerBase::ManagerBase(std::string_view module, Dispatcher& dispatcher)
  : m_module(module)
  , m_dispatcher(dispatcher)
{
}

ManagerBase::ManagerBase(std::string_view module, Dispatcher& dispatcher,
                         CommandAuthenticator& authenticator)
  : m_module(module)
  , m_dispatcher(dispatcher)
  , m_authenticator(&authenticator)
{
}

ManagerBase::~ManagerBase() = default;

void
ManagerBase::registerStatusDatasetHandler(const std::string& verb,
                                          const ndn::mgmt::StatusDatasetHandler& handler)
{
  m_dispatcher.addStatusDataset(makeRelPrefix(verb),
                                ndn::mgmt::makeAcceptAllAuthorization(),
                                handler);
}

ndn::mgmt::PostNotification
ManagerBase::registerNotificationStream(const std::string& verb)
{
  return m_dispatcher.addNotificationStream(makeRelPrefix(verb));
}

std::string
ManagerBase::extractSigner(const Interest& interest)
{
  try {
    // try v0.3 format first
    auto sigInfo = interest.getSignatureInfo();
    if (!sigInfo) {
      // fallback to v0.2 format
      sigInfo.emplace(interest.getName().at(ndn::signed_interest::POS_SIG_INFO).blockFromValue());
    }
    return sigInfo->getKeyLocator().getName().toUri();
  }
  catch (const tlv::Error&) {
    return "";
  }
}

ndn::mgmt::Authorization
ManagerBase::makeAuthorization(const std::string& verb)
{
  BOOST_ASSERT(m_authenticator != nullptr);
  return m_authenticator->makeAuthorization(m_module, verb);
}

} // namespace nfd
