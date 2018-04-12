package commandlog

import (
	"os"

	"github.com/spf13/afero"
)

// CommandLog Represents an instance of a command log.
type CommandLog struct {
	fs   afero.Fs
	file afero.File
}

// NewInMemory returns a new instance of CommandLog with an in memory log file.
func NewInMemory() *CommandLog {
	return new(true)
}

// NewOnDisk returns a new instance of CommandLog with an on disk log file.
func NewOnDisk() *CommandLog {
	return new(false)
}

// new Returns a new instance of CommandLog.
func new(inmemory bool) *CommandLog {
	var fs afero.Fs
	if inmemory {
		fs = afero.NewMemMapFs()
	} else {
		fs = afero.NewOsFs()
	}
	return &CommandLog{
		fs: fs,
	}
}

// Open opens the specified log file for reading and writing.
func (log *CommandLog) Open(path string, perm os.FileMode) error {
	var err error

	exists, _ := afero.Exists(log.fs, path)
	if exists {
		log.file, err = log.fs.OpenFile(path, os.O_RDWR, perm)
		if err != nil {
			return err
		}
	} else {
		log.file, err = log.fs.Create(path)
		if err != nil {
			return err
		}
		err = log.fs.Chmod(path, perm)
		if err != nil {
			return err
		}
	}
	return nil
}

// Close closes the command log file on disk.
func (log *CommandLog) Close() error {
	return log.file.Close()
}

// Sync flushes and fsyncs the command log file to disk.
func (log *CommandLog) Sync() error {
	return log.file.Sync()
}

// Append appends a new record to the command log.
func (log *CommandLog) Append(data []byte) (bytes int, err error) {
	bytes, err = log.file.Write(data)
	return bytes, err
}
