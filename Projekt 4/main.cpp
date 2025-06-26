#define UNICODE
#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#undef max
#include <atomic>
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// Długości segmentów ramienia
const float L1 = 150.0f;
const float L2 = 100.0f;

// Punkt podstawy ramienia
const PointF BASE{300.0f, 450.0f};

// Dostępne kształty klocków
enum class Shape
{
    Square,
    Circle,
    Triangle,
    Rectangle
};

// Dostępne tryby automatyczne
enum class Operations
{
    Default,
    Tower6,
    Tower3,
    SortWeightUp,
    SortWeightDown,
    SortSizeUp,
    SortSizeDown,
    SortSizePyramid
};

// Wektory do przechowywania identyfikatorów klocków
std::vector<int> blocks_IDs;
std::vector<int> blocks_IDs_copy;

// Opis pojedynczego klocka
struct Block
{
    const int ID;
    PointF pos;
    float size;
    Shape shape;
    float weight;
    bool isHeld = false;

    static int generateID()
    {
        static std::atomic<int> counter{1};
        blocks_IDs.push_back(counter);
        blocks_IDs_copy.push_back(counter);
        return counter++;
    }

    Block(PointF pos = {0.0f, 0.0f},
          float size = 0.0f,
          Shape shape = Shape::Square,
          float weight = 0.0f,
          bool isHeld = false)
        : ID(generateID()),
          pos(pos),
          size(size),
          shape(shape),
          weight(weight),
          isHeld(isHeld)
    {
    }
};

// Pojedynczy ruch (kąty + flagi podniesienia/łożenia)
struct Move
{
    float a1, a2;
    bool pick, drop;
};

HINSTANCE g_hInst;
ULONG_PTR g_gdiToken;
std::vector<Block> g_blocks;
std::vector<Move> g_moves;

// Aktualne kąty ramienia
float g_angle1 = 30, g_angle2 = 30;
float DEFAULT_R1_ANGLE = 30;
float DEFAULT_R2_ANGLE = 30; // Domyślny kąt drugiego segmentu
bool g_holding = false;
Block *g_heldBlock = nullptr;

// Nagrywanie / odtwarzanie ruchów
bool g_record = false;
bool g_autoPlay = false;
size_t g_playIndex = 0;
int g_timerDelay = 10; // opóźnienie timera w ms

// Maksymalny / minimalny udźwig ramienia
float g_maxLift = 7.0f;
float g_minLift = 1.0f;

static float r1_pick = -1, r2_pick = -1;
static float r1_place = -1, r2_place = -1;

// Ograniczenia kątowe ramienia
float angle1_min = 0.0f;
float angle1_max = 180.0f;
float angle2_min = -170.0f;
float angle2_max = 170.0f;

// Domyślny klocek
float default_block_weight = 2.0f;
float default_block_size = 30.0f;
float max_block_weight = 50.0f;
float max_block_size = 60.0f;
float min_block_size = 20.0f;

// Ruch automatyczny
float r1_target_angle;
float r2_target_angle;
int target_ID = -1;
Operations operation_state = Operations::Default;
bool block_unreachable = false;
int last_skipped_block = -1;
const float tower_X = 450.0f;
float g_new_tower_height = 0.0f;
int g_tower_phase = 0; // 0: podnoszenie, 1: przenoszenie, 2: odkładanie

// Definicje prostokątów przycisków
Gdiplus::RectF weight_plus(10.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF weight_minus(10.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF size_plus(95.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF size_minus(95.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF tower_three(180.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF tower_six(180.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF weight_sort_up(265.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF weight_sort_down(265.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF size_sort_up(350.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF size_sort_down(350.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF size_sort_pyramid(435.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF clear_board(435.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF draw_square(520.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF draw_circle(520.0f, 505.0f, 80.0f, 40.0f);
Gdiplus::RectF draw_rectangle(605.0f, 460.0f, 80.0f, 40.0f);
Gdiplus::RectF draw_triangle(605.0f, 505.0f, 80.0f, 40.0f);

// Oblicza pozycję końcówki ramienia
PointF ComputeEnd(float a1, float a2)
{
    // Konwersja kątów na radiany
    float angle1_rad = a1 * M_PI / 180.0f;
    float angle2_rad = a2 * M_PI / 180.0f;

    // Pozycja łokcia
    float x1 = BASE.X + L1 * cos(angle1_rad);
    float y1 = BASE.Y - L1 * sin(angle1_rad); // Uwaga: minus, bo Y rośnie w dół

    // Zabezpieczenie przed wyjściem poza obszar roboczy
    if (y1 > BASE.Y || std::isnan(x1) || std::isnan(y1))
    {
        return {-1, -1}; // Wskaż błąd
    }

    // Pozycja końcówki
    float x2 = x1 + L2 * cos(angle1_rad + angle2_rad);
    float y2 = y1 - L2 * sin(angle1_rad + angle2_rad); // Uwaga: minus

    // Dodatkowa weryfikacja
    if (y2 > BASE.Y || std::isnan(x2) || std::isnan(y2))
    {
        return {-1, -1}; // Wskaż błąd
    }

    return {x2, y2};
}

// Zwraca prostokąt graniczny klocka
RectF GetBlockBoundingBox(const Block &b)
{
    switch (b.shape)
    {
    case Shape::Square:
        return RectF(b.pos.X - b.size / 2, b.pos.Y - b.size / 2, b.size, b.size);
    case Shape::Rectangle:
        return RectF(b.pos.X - b.size * 0.75f, b.pos.Y - b.size / 2, b.size * 1.5f, b.size);
    case Shape::Circle:
        return RectF(b.pos.X - b.size / 2, b.pos.Y - b.size / 2, b.size, b.size);
    case Shape::Triangle:
    {
        // Wysokość trójkąta równobocznego
        float height = (sqrt(3.0f) / 2.0f * b.size);
        return RectF(b.pos.X - b.size / 2, b.pos.Y - height / 2, b.size, height);
    }
    default:
        return RectF(b.pos.X - b.size / 2, b.pos.Y - b.size / 2, b.size, b.size);
    }
}

// Oblicza odwrotną kinematykę dla wskazanego punktu
bool CalculateIK(PointF target, float &a1, float &a2)
{
    // Cel nie może być poniżej poziomu podłogi
    if (target.Y > BASE.Y)
    {
        return false;
    }

    // Przekształcenie układu współrzędnych względem podstawy
    float dx = target.X - BASE.X;
    float dy = BASE.Y - target.Y;

    // Sprawdzenie, czy cel jest osiągalny z zakresu ramienia
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > L1 + L2 || distance < fabs(L1 - L2))
    {
        return false;
    }

    // Kąt między segmentami
    float cos_theta2 = (dx * dx + dy * dy - L1 * L1 - L2 * L2) / (2 * L1 * L2);
    cos_theta2 = std::clamp(cos_theta2, -1.0f, 1.0f);
    float theta2 = acos(cos_theta2);

    // Obliczenie kąta pierwszego segmentu
    float k1 = L1 + L2 * cos_theta2;
    float k2 = L2 * sin(theta2);
    float theta1 = atan2(dy, dx) - atan2(k2, k1);

    // Konwersja na stopnie
    a1 = theta1 * 180.0f / M_PI;
    a2 = theta2 * 180.0f / M_PI;

    // Normalizacja kątów
    if (a1 < 0)
        a1 += 360;
    if (a2 < 0)
        a2 += 360;

    // Sprawdzenie ograniczeń kątowych
    if (a1 < angle1_min || a1 > angle1_max || a2 < angle2_min || a2 > angle2_max)
    {
        return false;
    }

    return true;
}

// Przygotowuje kąty docelowe dla danego klocka
void CalculateTargetAngles(int block_ID)
{
    r1_target_angle = -1;
    r2_target_angle = -1;
    block_unreachable = false;
    float a1, a2;

    for (auto &b : g_blocks)
    {
        if (b.ID == block_ID)
        {
            // Sprawdzenie, czy klocek nie leży na podłodze
            if (b.pos.Y > BASE.Y - 5)
            {
                block_unreachable = true;
                return;
            }

            // Ustal punkt 5 px nad klockiem
            PointF targetAboveBlock = b.pos;
            targetAboveBlock.Y -= 5.0f;

            if (!CalculateIK(targetAboveBlock, a1, a2))
            {
                block_unreachable = true;
                return;
            }

            // Weryfikacja pozycji końcówki
            PointF end = ComputeEnd(a1, a2);
            if (end.X < 0 || end.Y < 0 || end.Y > BASE.Y)
            {
                block_unreachable = true;
                return;
            }

            r1_target_angle = a1;
            r2_target_angle = a2;
            return;
        }
    }
}

// Rysuje pojedynczy klocek
void DrawBlock(Graphics &g, const Block &b)
{
    SolidBrush br(Color(200, 50, 50));

    switch (b.shape)
    {
    case Shape::Square:
    {
        float x = b.pos.X - b.size * 0.5f;
        float y = b.pos.Y - b.size * 0.5f;
        g.FillRectangle(&br, x, y, b.size, b.size);
        break;
    }
    case Shape::Rectangle:
    {
        float rectWidth = b.size * 1.5f;
        float rectHeight = b.size;
        float x = b.pos.X - rectWidth * 0.5f;
        float y = b.pos.Y - rectHeight * 0.5f;
        g.FillRectangle(&br, x, y, rectWidth, rectHeight);
        break;
    }
    case Shape::Circle:
    {
        float x = b.pos.X - b.size * 0.5f;
        float y = b.pos.Y - b.size * 0.5f;
        g.FillEllipse(&br, x, y, b.size, b.size);
        break;
    }
    case Shape::Triangle:
    {
        PointF pts[3] = {
            {b.pos.X, b.pos.Y - (2 * b.size) / 3},
            {b.pos.X - b.size / 2, b.pos.Y + b.size / 3},
            {b.pos.X + b.size / 2, b.pos.Y + b.size / 3}};
        g.FillPolygon(&br, pts, 3);
        break;
    }
    }

    // Rysuje etykietę z wagą klocka
    std::wstring w = L"w:" + std::to_wstring(static_cast<int>(b.weight));
    Font font(L"Arial", 10);
    SolidBrush tb(Color::Black);
    g.DrawString(w.c_str(), -1, &font, b.pos, &tb);
}

// Rysuje przycisk z tekstem
void DrawButton(Gdiplus::Graphics &graphics, const Gdiplus::RectF &rect, const std::wstring &text)
{
    Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(220, 220, 220));
    Gdiplus::Pen borderPen(Gdiplus::Color(0, 0, 0));

    graphics.FillRectangle(&backgroundBrush, rect);
    graphics.DrawRectangle(&borderPen, rect.X, rect.Y, rect.Width, rect.Height);

    Gdiplus::Font font(L"Arial", 10);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(0, 0, 0));
    Gdiplus::StringFormat stringFormat;
    stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
    stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    graphics.DrawString(text.c_str(), -1, &font, rect, &stringFormat, &textBrush);
}

// Tworzy wieżę z 6 prostokątnych klocków po lewej
void BuildTower()
{
    g_blocks.clear();
    float startY = 450.0f;
    for (int i = 0; i < 6; i++)
    {
        Block b;
        b.size = 30;
        b.shape = Shape::Rectangle;
        b.weight = 0.5f + i; // wagi: 0.5, 1.5, 2.5, 3.5, 4.5, 5.5
        b.pos = {150.0f, startY - (i == 0 ? b.size / 2 : (b.size / 2 + i * (b.size + 2)))};
        g_blocks.push_back(b);
    }
}

// Sprawdza, czy kąty są równe w zadanym tolerancja
bool anglesEqual(float a, float b, float tolerance = 1.0f)
{
    float diff = fabs(a - b);
    return diff < tolerance || fabs(diff - 360.0f) < tolerance;
}

// Sprawdza wysokość obiektu trzymanego
int HoldingObjectCheck()
{
    if (!g_holding)
        return 0;
    return (g_heldBlock->size) / 2;
}

float getBlockWeight(int blockID)
{
    for (const auto &b : g_blocks)
    {
        if (b.ID == blockID)
        {
            return b.weight;
        }
    }
    // jeśli nie znaleziono klocka
    return -1.0f;
}

// Bezpieczny krok w kierunku docelowego kąta
auto SafeStep = [&](float &cur, float target, float otherAngle, bool isFirstSegment)
{
    float step = (target > cur ? +1.0f : -1.0f);
    if (fabs(cur - target) < 1.0f)
    {
        cur = target;
        return;
    }
    float next = cur + step;
    // Symulacja przyszłej pozycji końcówki
    float a1 = isFirstSegment ? next : g_angle1;
    float a2 = isFirstSegment ? g_angle2 : next;
    PointF end = ComputeEnd(a1, a2);
    // Ruch dozwolony, jeśli nie wjedzie w podłogę
    if (end.Y <= BASE.Y - HoldingObjectCheck())
    {
        // I mieści się w ograniczeniach kątowych
        if (isFirstSegment)
            cur = std::clamp(next, angle1_min, angle1_max);
        else
            cur = std::clamp(next, angle2_min, angle2_max);
    }
};

// Przesuwa oba segmenty w stronę ustawionych celów
auto MoveTowards = [&]()
{
    SafeStep(g_angle1, (r1_pick >= 0 && g_tower_phase == 0) ? r1_pick : r1_place, g_angle2, true);
    SafeStep(g_angle2, (r1_pick >= 0 && g_tower_phase == 0) ? r2_pick : r2_place, g_angle1, false);
};

// Sprawdza, czy kliknięto w dany przycisk
bool isButtonClicked(const Gdiplus::RectF &button, const Gdiplus::PointF &p)
{
    return p.X >= button.X && p.X <= button.X + button.Width &&
           p.Y >= button.Y && p.Y <= button.Y + button.Height;
}

// Procedura obsługi wiadomości okna
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, g_timerDelay, nullptr);
        break;

    case WM_TIMER:
        // Odtwarzanie nagranych ruchów
        if (g_autoPlay && g_playIndex < g_moves.size())
        {
            auto &m = g_moves[g_playIndex++];
            g_angle1 = m.a1;
            g_angle2 = m.a2;
            // symulacja podniesienia
            if (m.pick && !g_holding)
            {
                PointF end = ComputeEnd(g_angle1, g_angle2);
                for (auto &b : g_blocks)
                {
                    float dx = end.X - b.pos.X;
                    float dy = end.Y - b.pos.Y;
                    if (!b.isHeld && sqrtf(dx * dx + dy * dy) < b.size * 0.6f)
                    {
                        if (b.weight <= g_maxLift)
                        {
                            b.isHeld = true;
                            g_holding = true;
                            g_heldBlock = &b;
                        }
                        break;
                    }
                }
            }
            // symulacja odłożenia
            if (m.drop && g_holding)
            {
                g_heldBlock->isHeld = false;
                g_heldBlock->pos = ComputeEnd(g_angle1, g_angle2);
                g_holding = false;
                g_heldBlock = nullptr;
            }
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        // Koniec trybu automatycznego
        if (g_autoPlay && g_playIndex == g_moves.size())
        {
            g_autoPlay = !g_autoPlay;
        }
        // Obsługa operacji Wieża6
        if (operation_state == Operations::Tower6 ||
            operation_state == Operations::Tower3)
        {
            if (!blocks_IDs_copy.empty())
            {
                Block *b = nullptr;
                // Jeśli nie wyznaczono target_ID, wybierz pierwszy
                if (target_ID == -1)
                {
                    target_ID = blocks_IDs_copy.front();
                    for (auto &bb : g_blocks)
                        if (bb.ID == target_ID)
                        {
                            b = &bb;
                            break;
                        }
                    if (b)
                    {
                        // 1) kąty do podniesienia (5 px nad klockiem)
                        PointF abovePick = {b->pos.X, b->pos.Y - 5.0f};
                        CalculateIK(abovePick, r1_pick, r2_pick);
                        // 2) kąty do odłożenia (5 px nad miejscem w wieży)
                        float blockH = GetBlockBoundingBox(*b).Height;
                        PointF dropPos = {tower_X, BASE.Y - (g_new_tower_height + blockH / 2)};
                        PointF abovePlace = {dropPos.X, dropPos.Y - 5.0f};
                        if (!CalculateIK(abovePlace, r1_place, r2_place))
                        {
                            // awaryjny powrót do pozycji domyślnej
                            r1_place = DEFAULT_R1_ANGLE;
                            r2_place = DEFAULT_R2_ANGLE;
                        }
                        g_tower_phase = 0;
                    }
                }

                // Faza 0 – ruch do podniesienia
                if (g_tower_phase == 0)
                {
                    MoveTowards();
                    if (anglesEqual(g_angle1, r1_pick) && anglesEqual(g_angle2, r2_pick))
                    {
                        for (auto &bb : g_blocks)
                        {
                            if (bb.ID == target_ID && !bb.isHeld)
                            {
                                bb.isHeld = true;
                                g_holding = true;
                                g_heldBlock = &bb;
                                break;
                            }
                        }
                        g_tower_phase = 1;
                    }
                }
                // Faza 1 – ruch do odłożenia
                else if (g_tower_phase == 1)
                {
                    MoveTowards();
                    if (anglesEqual(g_angle1, r1_place) && anglesEqual(g_angle2, r2_place))
                    {
                        if (g_holding && g_heldBlock)
                        {
                            RectF bbox = GetBlockBoundingBox(*g_heldBlock);
                            float bh = bbox.Height;
                            const float GAP = 2.0f;
                            PointF dropPos = {
                                tower_X,
                                BASE.Y - (g_new_tower_height + bh / 2 + GAP / 2)};
                            g_heldBlock->isHeld = false;
                            g_heldBlock->pos = dropPos;
                            g_holding = false;
                            g_heldBlock = nullptr;
                            g_new_tower_height += bh + GAP;
                        }
                        blocks_IDs_copy.erase(blocks_IDs_copy.begin());
                        target_ID = -1;
                        r1_pick = r2_pick = r1_place = r2_place = -1;
                        g_tower_phase = 0;
                    }
                }
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            else
            {
                // Zakończenie operacji
                operation_state = Operations::Default;
                g_new_tower_height = 0.0f;
                g_tower_phase = 0;
            }
        }
        break;

    case WM_KEYDOWN:
    {
        bool didPick = false, didDrop = false;
        float r1 = g_angle1 * M_PI / 180.0f;
        PointF p1{BASE.X + L1 * cosf(r1), BASE.Y - L1 * sinf(r1)};

        switch (wp)
        {
        case VK_UP:
            if ((g_angle1 + 2 <= angle1_max) &&
                (BASE.Y - L1 * sinf((g_angle1 + 2) * M_PI / 180.0f) -
                 L2 * sinf((g_angle1 + g_angle2 + 2) * M_PI / 180.0f)) <= BASE.Y - HoldingObjectCheck())
                g_angle1 += 2;
            break;
        case VK_DOWN:
            if ((g_angle1 - 2 >= angle1_min) &&
                (BASE.Y - L1 * sinf((g_angle1 - 2) * M_PI / 180.0f) -
                 L2 * sinf((g_angle1 + g_angle2 - 2) * M_PI / 180.0f)) <= BASE.Y - HoldingObjectCheck())
                g_angle1 -= 2;
            break;
        case VK_LEFT:
            if ((g_angle2 + 2 <= angle2_max) &&
                (p1.Y - L2 * sinf((g_angle1 + g_angle2 + 2) * M_PI / 180.0f)) <= BASE.Y - HoldingObjectCheck())
                g_angle2 += 2;
            break;
        case VK_RIGHT:
            if ((g_angle2 - 2 >= angle2_min) &&
                (p1.Y - L2 * sinf((g_angle1 + g_angle2 - 2) * M_PI / 180.0f)) <= BASE.Y - HoldingObjectCheck())
                g_angle2 -= 2;
            break;
        case 'P': // Podniesienie
            if (!g_holding)
            {
                PointF end = ComputeEnd(g_angle1, g_angle2);
                for (auto &b : g_blocks)
                {
                    float dx = end.X - b.pos.X;
                    float dy = end.Y - b.pos.Y;
                    if (!b.isHeld && sqrtf(dx * dx + dy * dy) < b.size * 0.6f)
                    {
                        if (b.weight <= g_maxLift && b.weight >= g_minLift)
                        {
                            b.isHeld = true;
                            g_holding = true;
                            g_heldBlock = &b;
                            didPick = true;
                        }
                        else
                        {
                            MessageBoxW(hWnd,
                                        (b.weight > g_maxLift ? L"Klocek za ciężki!" : L"Klocek za lekki"),
                                        L"Błąd",
                                        MB_OK);
                        }
                        break;
                    }
                }
            }
            else
            {
                MessageBoxW(hWnd,
                            L"Ramię nie człowiek, dwa klocki naraz to za dużo",
                            L"Błąd",
                            MB_OK);
            }
            break;
        case 'O': // Odłożenie
            if (g_holding)
            {
                g_heldBlock->isHeld = false;
                g_heldBlock->pos = ComputeEnd(g_angle1, g_angle2);
                g_holding = false;
                g_heldBlock = nullptr;
                didDrop = true;
            }
            break;
        case 'R': // Włącz/wyłącz nagrywanie
            g_record = !g_record;
            if (!g_record)
                MessageBoxW(hWnd, L"Zapisano ruchy.", L"Info", MB_OK);
            else
                g_moves.clear();
            break;
        case 'F': // Odtwarzanie/Pauza
            g_autoPlay = !g_autoPlay;
            if (g_autoPlay)
                g_playIndex = 0;
            break;
        case '+':
            g_timerDelay = std::max(10, g_timerDelay - 10);
            SetTimer(hWnd, 1, g_timerDelay, nullptr);
            break;
        case '-':
            g_timerDelay += 10;
            SetTimer(hWnd, 1, g_timerDelay, nullptr);
            break;
        default:
            break;
        }

        if (g_record)
            g_moves.push_back({g_angle1, g_angle2, didPick, didDrop});

        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lp);
        int y = GET_Y_LPARAM(lp);
        Gdiplus::PointF clickPt(static_cast<float>(x), static_cast<float>(y));

        if (isButtonClicked(weight_plus, clickPt) && (default_block_weight + 1 <= max_block_weight))
        {
            default_block_weight++;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (isButtonClicked(weight_minus, clickPt) && (default_block_weight - 1 > 0))
        {
            default_block_weight--;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (isButtonClicked(size_plus, clickPt) && (default_block_size + 1 <= max_block_size))
        {
            default_block_size++;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (isButtonClicked(size_minus, clickPt) && (default_block_size - 1 > min_block_size))
        {
            default_block_size--;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        // Wieża 6
        if (isButtonClicked(tower_six, clickPt) && !blocks_IDs.empty())
        {
            operation_state = Operations::Tower6;

            // skopiuj tylko pierwsze 6 ID bloków,
            // których waga zawiera się w [g_minLift, g_maxLift]
            blocks_IDs_copy.clear();
            for (int id : blocks_IDs)
            {
                float w = getBlockWeight(id);
                if (w >= g_minLift && w <= g_maxLift)
                {
                    blocks_IDs_copy.push_back(id);
                    if (blocks_IDs_copy.size() == 6)
                        break;
                }
            }

            target_ID = -1;
            r1_target_angle = r2_target_angle = -1;
            g_new_tower_height = 0.0f;
            g_tower_phase = 0;
        }
        // Wieża 3
        else if (isButtonClicked(tower_three, clickPt) && !blocks_IDs.empty())
        {
            operation_state = Operations::Tower3;

            // skopiuj tylko pierwsze 3 ID bloków,
            // których waga zawiera się w [g_minLift, g_maxLift]
            blocks_IDs_copy.clear();
            for (int id : blocks_IDs)
            {
                float w = getBlockWeight(id);
                if (w >= g_minLift && w <= g_maxLift)
                {
                    blocks_IDs_copy.push_back(id);
                    if (blocks_IDs_copy.size() == 3)
                        break;
                }
            }

            target_ID = -1;
            r1_target_angle = r2_target_angle = -1;
            g_new_tower_height = 0.0f;
            g_tower_phase = 0;
        }
        // Czyszczenie planszy
        if (isButtonClicked(clear_board, clickPt))
        {
            g_blocks.clear();
            blocks_IDs.clear();
            blocks_IDs_copy.clear();
            InvalidateRect(hWnd, nullptr, TRUE);
        }

        // Dodawanie klocków
        PointF spawnPos(150.0f, BASE.Y - default_block_size / 2);
        if (isButtonClicked(draw_square, clickPt))
        {
            Block b;
            b.shape = Shape::Square;
            b.size = default_block_size;
            b.weight = default_block_weight;
            b.pos = spawnPos;
            g_blocks.push_back(b);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (isButtonClicked(draw_circle, clickPt))
        {
            Block b;
            b.shape = Shape::Circle;
            b.size = default_block_size;
            b.weight = default_block_weight;
            b.pos = spawnPos;
            g_blocks.push_back(b);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (isButtonClicked(draw_rectangle, clickPt))
        {
            Block b;
            b.shape = Shape::Rectangle;
            b.size = default_block_size;
            b.weight = default_block_weight;
            b.pos = spawnPos;
            g_blocks.push_back(b);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (isButtonClicked(draw_triangle, clickPt))
        {
            Block b;
            b.shape = Shape::Triangle;
            b.size = default_block_size;
            b.weight = default_block_weight;
            b.pos = spawnPos;
            g_blocks.push_back(b);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Graphics g(hdc);

        // Tło białe
        g.Clear(Color::White);

        // Rysuj wszystkie niepodniesione klocki
        for (auto &b : g_blocks)
            if (!b.isHeld)
                DrawBlock(g, b);

        // Rysuj ramię
        Pen pen(Color::Black);
        Pen penJ(Color::Red, 6);
        float r1 = g_angle1 * M_PI / 180.0f;
        PointF p1{BASE.X + L1 * cosf(r1), BASE.Y - L1 * sinf(r1)};
        g.DrawLine(&pen, BASE, p1);

        float r2 = (g_angle1 + g_angle2) * M_PI / 180.0f;
        PointF p2{p1.X + L2 * cosf(r2), p1.Y - L2 * sinf(r2)};
        g.DrawLine(&pen, p1, p2);

        // Rysuj podłogę
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        Rect ground(clientRect.left, BASE.Y + 1, clientRect.right - clientRect.left, clientRect.bottom - BASE.Y);
        SolidBrush brush(Color::DarkGray);
        g.FillRectangle(&brush, ground);

        // Rysuj trzymany klocek przy końcówce
        if (g_holding && g_heldBlock)
        {
            Block tmp = *g_heldBlock;
            tmp.pos = p2;
            DrawBlock(g, tmp);
        }

        // Tekst statusu
        std::wstring info = std::wstring(L" Mode=") +
                            ((g_autoPlay || operation_state != Operations::Default) ? L"Auto" : (g_record ? L"Rec" : L"Manual")) +
                            L"; Delay=" + std::to_wstring(g_timerDelay) + L"ms" +
                            L"; Domyślnie waga=" + std::to_wstring((int)default_block_weight) +
                            L", rozmiar=" + std::to_wstring((int)default_block_size);
        Font f(L"Arial", 14);
        SolidBrush blackBrush(Color::Black);
        g.DrawString(info.c_str(), -1, &f, PointF(10, 10), &blackBrush);

        // Rysuj przyciski
        DrawButton(g, weight_plus, L"Waga +");
        DrawButton(g, weight_minus, L"Waga -");
        DrawButton(g, size_plus, L"Rozmiar +");
        DrawButton(g, size_minus, L"Rozmiar -");
        DrawButton(g, tower_six, L"Wieża 6");
        DrawButton(g, tower_three, L"Wieża 3");
        DrawButton(g, weight_sort_up, L"S Waga UP");
        DrawButton(g, weight_sort_down, L"S Waga DOWN");
        DrawButton(g, size_sort_up, L"S Rozmiar UP");
        DrawButton(g, size_sort_down, L"S Rozmiar DOWN");
        DrawButton(g, size_sort_pyramid, L"S Rozmiar PYRAMID");
        DrawButton(g, clear_board, L"Wyczyść");
        DrawButton(g, draw_square, L"Dodaj Kwadrat");
        DrawButton(g, draw_circle, L"Dodaj Koło");
        DrawButton(g, draw_rectangle, L"Dodaj Prostokąt");
        DrawButton(g, draw_triangle, L"Dodaj Trójkąt");

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        GdiplusShutdown(g_gdiToken);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

// Punkt wejścia: inicjalizacja GDI+, utworzenie wieży, okno i pętla komunikatów
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nShow)
{
    g_hInst = hInst;

    GdiplusStartupInput gdipIn;
    GdiplusStartup(&g_gdiToken, &gdipIn, nullptr);

    BuildTower();

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"RobotArm2D";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        wc.lpszClassName,
        L"Robot Arm Simulator",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        nullptr,
        nullptr,
        hInst,
        nullptr);

    ShowWindow(hWnd, nShow);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}