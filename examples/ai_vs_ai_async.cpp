#include "asyncgame.h"
#include "asyncaiplayer.h"
#include <thread>

using namespace std;
using namespace boost::asio;
int main()
{
    auto io_ptr = std::make_shared<io_service>();
    thread threads[5];


    shared_ptr<AsyncAiPlayer> players[2] = {make_shared<AsyncAiPlayer>(WHITE, 2), make_shared<AsyncAiPlayer>(WHITE, 2)};
    AsyncGame game(io_ptr, players[0], players[1]);
    game.start([io_ptr](AsyncPlayer::EndStatus end_status) {
        switch (end_status) {
        case AsyncPlayer::WHITE_WIN:
            cout << "White win" << endl;
            break;
        case AsyncPlayer::DRAW:
            cout << "Draw" << endl;
            break;
        case AsyncPlayer::WHITE_LOOSE:
            cout << "White loose" << endl;
            break;
        }
        io_ptr->stop();
    });
    for (thread & thrd : threads) {
        thrd = thread([io_ptr]() {
            io_ptr->run();
        });
    }

    for (thread & thrd : threads) {
        thrd.join();
    }
    return 0;
}
