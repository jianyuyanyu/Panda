#include "BaseApplication.hpp"

int Panda::BaseApplication::Initialize() {

  m_bQuit = false;

  return 0;
}


void Panda::BaseApplication::Finalize() {
  
}


void Panda::BaseApplication::Tick() {
}

bool Panda::BaseApplication::IsQuit() {
  return m_bQuit;
}
