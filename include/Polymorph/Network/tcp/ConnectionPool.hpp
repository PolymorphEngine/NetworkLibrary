/*
** EPITECH PROJECT, 2020
** ConnectionPool.hpp
** File description:
** header for ConnectionPool.c
*/


#pragma once

#include <set>
#include <vector>
#include "Polymorph/Network/tcp/IConnectionPool.hpp"
#include "Polymorph/Network/types.hpp"

namespace polymorph::network::tcp
{

    class ConnectionPool : public IConnectionPool
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ConnectionPool() = default;

            ~ConnectionPool() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            std::set<std::shared_ptr<ClientConnection>> _connections;
            std::mutex _connectionsMutex;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void join(std::shared_ptr<ClientConnection> connection) override;

            void leave(std::shared_ptr<ClientConnection> connection) override;

            std::vector<std::shared_ptr<ClientConnection>> getConnections();

            std::shared_ptr<ClientConnection> getConnectionBySessionId(SessionId id);



        private:


//////////////////////--------------------------/////////////////////////

    };
}
