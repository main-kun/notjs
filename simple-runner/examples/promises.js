let message = "Initial value";

Promise.resolve().then(() => {
    message = "Promise was processed!";
});

message;