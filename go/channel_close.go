// channel close example
// makes a channel of capacity 5, sends messages, and then close

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
  const capacity = 5
  messages := make(chan string, capacity)
  go func() {
    defer elapsed("send messages")()
    for i := 0; i<5; i++ {
      fmt.Printf("thread %d: about to ping\n",i)
      messages <- fmt.Sprintf("thread %d: ping",i)
      fmt.Printf("thread %d: ping done, closing channel\n", i)
      // uncomment to close before last message
      // to induce a panic
//      if i == 3 {
//        close(messages)
//      }
    }
    close(messages)
  }()

  defer elapsed("receiving messages")()
  fmt.Println("main: about to receive, waiting 2s..")
  time.Sleep(2 * time.Second)

  fmt.Println("main: receiving!")
  for msg := range messages {
    fmt.Println("got: " + msg)
  }

  fmt.Println("main: receive done")
}
