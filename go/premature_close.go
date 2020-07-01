package main

import (
  "fmt"
  "time"
)

func elapsed(what string) func() {
  start := time.Now()
  return func() {
    fmt.Printf("%s took %v\n", what, time.Since(start))
  }
}

func main() {

  messages := make(chan string)

  go func() {
    defer elapsed("send")()
    fmt.Println("thread: about to ping")
    messages <- "thread: ping"
    for i := 0; i<5; i++ {
      if i == 4 {
        close(messages)
      }
      messages <- fmt.Sprintf("ping %d", i)
    }
    fmt.Println("thread: ping done, closing channel")
  }()

  {
    defer elapsed("receive")()
    time.Sleep(2 * time.Second)
    fmt.Println("main: about to receive")

    for msg := range messages {
      fmt.Println("got: " + msg)
    }
    fmt.Println("main: receive done")
  }
  time.Sleep(10 * time.Millisecond)
}
