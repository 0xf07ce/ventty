//
// Interactive demo: Korean text rendering
// Tests halfwidth/fullwidth alignment
//

#include <ventty/ventty.h>

#include <chrono>
#include <thread>
#include <string>

using namespace ventty;

int main()
{
    Terminal term;
    if (!term.init())
        return 1;

    Style const cyan{Colors::CYAN, Colors::BLACK};
    Style const yellowBold{Colors::YELLOW, Colors::BLACK, Attr::Bold};
    Style const white{Colors::WHITE, Colors::BLACK};
    Style const green{Colors::GREEN, Colors::BLACK};
    Style const gray{Colors::LIGHT_GRAY, Colors::BLACK};
    Style const red{Colors::RED, Colors::BLACK};
    Style const blue{Colors::BLUE, Colors::BLACK};
    Style const yellow{Colors::YELLOW, Colors::BLACK};
    Style const darkGray{Colors::DARK_GRAY, Colors::BLACK};

    auto * mainWin = term.createWindow(2, 1, 50, 20);
    mainWin->drawBox(cyan);
    mainWin->drawText(2, 0, " 한글 테스트 ", yellowBold);

    // Halfwidth / Fullwidth alignment test
    mainWin->drawText(2, 2,  "ABCDEFGHIJ", white);
    mainWin->drawText(2, 3,  "가나다라마", green);
    mainWin->drawText(2, 4,  "0123456789", gray);
    mainWin->drawText(2, 5,  "아아아아아", cyan);

    // Mixed text
    mainWin->drawText(2, 7,  "HP: 100/100  공격력: 15", red);
    mainWin->drawText(2, 8,  "MP: 50/80    방어력: 8", blue);
    mainWin->drawText(2, 9,  "경험치: 1234/5000", yellow);

    // Attribute tests
    mainWin->drawText(2, 11, "Bold 볼드",
                      Style{Colors::WHITE, Colors::BLACK, Attr::Bold});
    mainWin->drawText(2, 12, "Underline 밑줄",
                      Style{Colors::WHITE, Colors::BLACK, Attr::Underline});
    mainWin->drawText(2, 13, "Italic 이탤릭",
                      Style{Colors::WHITE, Colors::BLACK, Attr::Italic});
    mainWin->drawText(2, 14, "Reverse 반전",
                      Style{Colors::WHITE, Colors::BLACK, Attr::Reverse});
    mainWin->drawText(2, 15, "Dim 디밍",
                      Style{Colors::WHITE, Colors::BLACK, Attr::Dim});
    mainWin->drawText(2, 16, "Strike 취소선",
                      Style{Colors::WHITE, Colors::BLACK, Attr::Strike});

    // Alignment ruler
    mainWin->drawText(2, 18, "||||||||||", darkGray);

    // Log window with scrollback
    auto * logWin = term.createWindow(2, 22, 50, 8);
    logWin->drawBox(Style{Colors::BROWN, Colors::BLACK});
    logWin->drawText(2, 0, " 메시지 로그 ", yellowBold);
    logWin->enableScroll(100);

    int msgIdx = 0;
    std::string messages[] = {
        "고블린이 나타났다!\n",
        "플레이어가 공격했다. 데미지: 15\n",
        "고블린이 반격했다. 데미지: 5\n",
        "포션을 사용했다. HP +20\n",
        "고블린을 처치했다! 경험치 +50\n",
        "레벨 업! Lv.2 달성!\n",
        "새로운 스킬: 화염구 습득\n",
        "던전 2층으로 내려갑니다...\n",
    };
    int const MSG_COUNT = 8;

    auto * innerLog = term.createWindow(3, 23, 48, 6);
    innerLog->enableScroll(100);
    innerLog->setZOrder(5);

    term.onKey([&](KeyEvent const & ev)
    {
        if (ev.key == KeyEvent::Key::Escape ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U'q'))
        {
            term.quit();
            return;
        }

        if (ev.key == KeyEvent::Key::Enter ||
            (ev.key == KeyEvent::Key::Char && ev.ch == U' '))
        {
            innerLog->print(messages[msgIdx % MSG_COUNT], white);
            msgIdx++;
        }

        if (ev.key == KeyEvent::Key::Up)
            innerLog->scroll(1);
        if (ev.key == KeyEvent::Key::Down)
            innerLog->scroll(-1);
    });

    // Status
    Style const statusStyle{Colors::WHITE, Colors::GREEN, Attr::Bold};
    term.fill(0, term.rows() - 1, term.cols(), 1, U' ', Colors::BLACK, Colors::GREEN);
    term.drawText(1, term.rows() - 1,
                 "Space/Enter: 메시지 추가 | Up/Down: 스크롤 | q: 종료",
                 statusStyle);

    while (term.isRunning())
    {
        while (term.pollEvent())
            ;

        term.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    term.shutdown();
    return 0;
}
