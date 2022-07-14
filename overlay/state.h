enum State {
  State_None = 0,
  State_Initialized = 1 << 0,
  State_Close = 1 << 1,
  State_Screenshot = 1 << 2,
  State_Record = 1 << 3,
};

static int Global_State;

bool GetState(int state);
void SetState(int state);
void RemoveState(int state);