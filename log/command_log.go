package log

import "os"

type CommandLog struct {
	file *os.File
}

func NewCommandLog() *CommandLog {
	return &CommandLog{}
}

func (log *CommandLog) Open(filename string) (err error) {
	log.file, err = os.OpenFile(filename, os.O_APPEND|os.O_WRONLY, 0600)
	return err
}

func (log *CommandLog) Close() error {
	return log.file.Close()
}

func (log *CommandLog) Append(entry *Entry) (err error) {
	_, err = log.file.Write(entry.Data)
	return err
}
