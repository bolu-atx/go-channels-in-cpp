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
  {
    defer elapsed("spawning send threads")()
    for i := 0; i<5; i++ {
      go func(i int) {
        fmt.Printf("thread %d: about to ping\n",i)
        messages <- fmt.Sprintf("thread %d: ping",i)
        fmt.Printf("thread %d: ping done, closing channel\n", i)
      }(i)
    }
  }

  {
    defer elapsed("receiving messages")()
    fmt.Println("main: about to receive, waiting 2s..")
    time.Sleep(2 * time.Second)

    fmt.Println("main: receiving!")
    for msg := range messages {
      fmt.Println("got: " + msg)
    }
    fmt.Println("main: receive done")
  }
  time.Sleep(10 * time.Millisecond)
}
