bool GetState(int state) {
  return Global_State & state;
}

void SetState(int state) {
  Global_State |= state;
}

void RemoveState(int state) {
  Global_State &= ~state;
}