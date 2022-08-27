// std::mutex state_mutex;

bool GetState(int state) {
  // std::lock_guard<std::mutex> lock(state_mutex);
  return Global_State & state;
}

void SetState(int state) {
  // std::lock_guard<std::mutex> lock(state_mutex);
  Global_State |= state;
}

void RemoveState(int state) {
  // std::lock_guard<std::mutex> lock(state_mutex);
  Global_State &= ~state;
}