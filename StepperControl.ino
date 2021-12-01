//place the stepper functions here

void heading(int steps){
  hStepper.step(steps);
}

void pitch(int steps){
  pStepper.step(steps);
}
