#ifndef _COMMUNICATE_LT_SESSION_CLI_SAFE_H
#define _COMMUNICATE_LT_SESSION_CLI_SAFE_H


#include "lt_session_cli.h"

class lt_session_cli_set;

class lt_session_cli_safe : public lt_session_cli, public lt_session_callback
{
private:
    std::mutex conn_m;
    bool is_down_connected;
    std::atomic<int> out_connect_ref;
    lt_session_callback *cb;
    lt_session_cli_set *set;
private:
    void connect_first(const std::string &ip, unsigned short port);

    void reconnect(const std::string &ip, unsigned short port);

    void disconnect_last();

protected:
    void disconnected() override;

public:
    void connected(lt_session *sess) override;

public:
    void rcv_done(lt_session *sess, lt_data_t *received_data, int error) override;

    void snd_done(lt_session *sess, lt_data_t *sent_data, int error) override;

    void disconnected(lt_session *sess) override;

public:
    lt_session_cli_safe(lt_session_cli_set *_set, boost::asio::io_service *_io_service, lt_session_callback *_cb);

    void connect(const std::string &ip, unsigned short port) override;

    void disconnect() override;

    void rcv(lt_data_t *data) override;

    void snd(lt_data_t *data) override;
};


#endif
