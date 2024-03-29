#include "Game.h"

Game::Game()
        : _screen_width(constants::screen_width), _screen_height(constants::screen_height),
          _running(true), _entity_view_creator(new EntityViewCreator), _entity_audio_creator(new EntityAudioCreator),
          _draw_hitbox(false) {
    _window = std::make_unique<sf::RenderWindow>(sf::VideoMode(_screen_width, _screen_height), "GameEngine");

    // coreAPI initialize
    _world = std::make_unique<World>(0, _screen_width, _screen_height, 0, _entity_view_creator, _entity_audio_creator);
}

Game::~Game() = default;

void Game::run() {
    while (_running) {
        // coreAPI sleep
        _world->sleep();

        // sfml events (window & keyboard)
        handleEvents();

        // coreAPI physicsUpdate
        _world->update();

        // sfml draw
        draw();
    }
}

void Game::draw() {
    _window->clear(sf::Color(127, 128, 118));

    // render entity views
    for (const auto &entity_sprite_view: _entity_view_creator->getEntitySpriteViews()) {
        _window->draw(entity_sprite_view->getSprite());

        if (_draw_hitbox) {
            // debug information
            _window->draw(entity_sprite_view->getHitbox());
            _window->draw(entity_sprite_view->getRays());
        }
    }
    for (const auto &entity_text_view: _entity_view_creator->getEntityTextViews()) {
        sf::Text new_text = entity_text_view->getText();
        _window->draw(new_text);

        if (_draw_hitbox) {
            // debug information
            _window->draw(entity_text_view->getHitbox());
            _window->draw(entity_text_view->getRays());
        }
    }

    _window->display();
}

void Game::handleEvents() {
    // coreAPI handle events (add to eventmanager)

    sf::Event event{};
    while (_window->pollEvent(event)) {
        // text typing
        if (_world->getUserInputMap()->get_input_stream && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;

            if (c != 0x08 && _world->getUserInputMap()->input_stream.size() != 150)
                _world->getUserInputMap()->input_stream += event.text.unicode;
            else if (c == 0x08 && !_world->getUserInputMap()->input_stream.empty()) {
                _world->getUserInputMap()->input_stream.pop_back();
            }
        }

        // button presses
        switch (event.type) {
            case sf::Event::Closed:
                _window->close();
                _running = false;
                break;

            case sf::Event::Resized:
                resizeWindow(_window->getSize().x, _window->getSize().y);
                break;

            case sf::Event::KeyPressed:
                handleKeyboardInput(event, true);
                break;

            case sf::Event::KeyReleased:
                handleKeyboardInput(event, false);
                break;

            case sf::Event::MouseButtonPressed:
                handleMouseInput(event, true);
                break;

            case sf::Event::MouseButtonReleased:
                handleMouseInput(event, false);
                break;

            case sf::Event::MouseMoved:
                handleMouseMovement();
                break;

            default:
                break;
        }
    }
}

void Game::handleKeyboardInput(const sf::Event &event, bool pressed) {
    switch (event.key.code) {
        case sf::Keyboard::A:
            _world->getUserInputMap()->a = pressed;
            break;

        case sf::Keyboard::B:
            _world->getUserInputMap()->b = pressed;
            break;

        case sf::Keyboard::C:
            _world->getUserInputMap()->c = pressed;
            break;

        case sf::Keyboard::D:
            _world->getUserInputMap()->d = pressed;
            break;

        case sf::Keyboard::E:
            _world->getUserInputMap()->e = pressed;
            break;

        case sf::Keyboard::F:
            _world->getUserInputMap()->f = pressed;
            break;

        case sf::Keyboard::G:
            _world->getUserInputMap()->g = pressed;
            break;

        case sf::Keyboard::H:
            _world->getUserInputMap()->h = pressed;
            if (pressed) {
                _draw_hitbox = !_draw_hitbox;
            }
            break;

        case sf::Keyboard::I:
            _world->getUserInputMap()->i = pressed;
            break;

        case sf::Keyboard::J:
            _world->getUserInputMap()->j = pressed;
            break;

        case sf::Keyboard::K:
            _world->getUserInputMap()->k = pressed;
            break;

        case sf::Keyboard::L:
            _world->getUserInputMap()->l = pressed;
            break;

        case sf::Keyboard::M:
            _world->getUserInputMap()->m = pressed;
            break;

        case sf::Keyboard::N:
            _world->getUserInputMap()->n = pressed;
            break;

        case sf::Keyboard::O:
            _world->getUserInputMap()->o = pressed;
            break;

        case sf::Keyboard::P:
            _world->getUserInputMap()->p = pressed;
            break;

        case sf::Keyboard::Q:
            _world->getUserInputMap()->q = pressed;
            break;

        case sf::Keyboard::R:
            _world->getUserInputMap()->r = pressed;
            break;

        case sf::Keyboard::S:
            _world->getUserInputMap()->s = pressed;
            break;

        case sf::Keyboard::T:
            _world->getUserInputMap()->t = pressed;
            break;

        case sf::Keyboard::U:
            _world->getUserInputMap()->u = pressed;
            break;

        case sf::Keyboard::V:
            _world->getUserInputMap()->v = pressed;
            break;

        case sf::Keyboard::W:
            _world->getUserInputMap()->w = pressed;
            break;

        case sf::Keyboard::X:
            _world->getUserInputMap()->x = pressed;
            break;

        case sf::Keyboard::Y:
            _world->getUserInputMap()->y = pressed;
            break;

        case sf::Keyboard::Z:
            _world->getUserInputMap()->z = pressed;
            break;

        case sf::Keyboard::Escape:
            _world->getUserInputMap()->esc = pressed;
            break;

        case sf::Keyboard::Slash:
            _world->getUserInputMap()->slash = pressed;
            break;

        default:
            break;
    }
}

void Game::handleMouseInput(const sf::Event &event, bool pressed) {
    switch (event.mouseButton.button) {
        case sf::Mouse::Left:
            _world->getUserInputMap()->mouse_button_left = pressed;
            _world->getUserInputMap()->mouse_button_left_clicked = false;
            break;

        case sf::Mouse::Middle:
            _world->getUserInputMap()->mouse_button_middle = pressed;
            break;

        case sf::Mouse::Right:
            _world->getUserInputMap()->mouse_button_right = pressed;
            break;

        default:
            break;
    }
}

void Game::handleMouseMovement() {
    _world->updateMousePosition(static_cast<float>(sf::Mouse::getPosition(*_window).x),
                                static_cast<float>(sf::Mouse::getPosition(*_window).y));
}

void Game::resizeWindow(unsigned int screen_width, unsigned int screen_height) {
    _screen_width = screen_width;
    _screen_height = screen_height;

    // update the view to the new screen_ui_size of the window
    sf::FloatRect visibleArea(0, 0, static_cast<float>(screen_width), static_cast<float>(screen_height));
    _window->setView(sf::View(visibleArea));

    _world->updateScreenResolution(0, static_cast<float>(screen_width), static_cast<float>(screen_height), 0);
}