#include <iostream>
#include <olc_net.h>

enum class CustomTypes : uint32_t {
    FireBullet,
    MovePlayer
};
]

class CustomClient : public olc::net::client_interface<CustomTypes> {
public:
    bool FireBullet(float x, float y) {
        olc::net::message<CustomTypes> msg;
        msg.header.id = CustomTypes::FireBullet;
        msg << x << y;
        Send(msg);
    }
};


int main() {
    CustomClient c;
    c.Connect("community.onelonecoder.com", 60000);
    c.FireBullet(2.0f, 5.0f);

    return 0;
}