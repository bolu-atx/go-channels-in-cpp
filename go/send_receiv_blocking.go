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
	fmt.Println("thread: about to ping\n")
    messages <- "thread: ping"
	fmt.Println("thread: ping done\n")
  }()

{
  defer elapsed("receive")()
  time.Sleep(2 * time.Second)
  fmt.Println("main: about to receive\n")
  msg := <-messages
  fmt.Println("main: receive done;\n")
  fmt.Println("got: " + msg)
}
  time.Sleep(10 * time.Millisecond)
}
