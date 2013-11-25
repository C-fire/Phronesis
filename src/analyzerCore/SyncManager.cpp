/*
Phronesis, a diagnosis and recovery tool for system administrators.
Copyright (C) 2013 Christophe Haen

This file is part of Phronesis.

Phronesis is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
any later version.

Phronesis is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Phronesis. If not, see <http://www.gnu.org/licenses/>.
*/


#include "SyncManager.hpp"

SyncManager::SyncManager()
{
    sem_init(&M_read, 0, 1);
    sem_init(&M_write, 0, 1);
    sem_init(&S_write, 0, 1);
    readers = 0;
}


SyncManager::~SyncManager()
{
    sem_destroy(&M_read);
    sem_destroy(&M_write);
    sem_destroy(&S_write);

}


void SyncManager::start_write ()
{
    sem_wait(&M_write);
    sem_wait(&S_write);
}

void SyncManager::end_write()
{
    sem_post(&M_write);
    sem_post(&S_write);
}

void SyncManager::start_read()
{
    sem_wait(&M_read);
    readers ++;
    if ( readers == 1)
    {
        sem_wait(&S_write);
    }
    sem_post(&M_read);
}


void SyncManager::end_read()
{
    sem_wait(&M_read);
    readers --;
    if ( readers == 0)
    {
        sem_post(&S_write);
    }
    sem_post(&M_read);
}
