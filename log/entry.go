package log

import "github.com/rs/xid"

type Entry struct {
	Id   xid.ID
	data []byte
}

func NewEntry() *Entry {
	return &Entry{
		Id: xid.New(),
	}
}
