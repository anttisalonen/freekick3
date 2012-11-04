#!/usr/bin/env python2
# coding: utf-8

import sys
import os
import glob
from lxml import etree
import random

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST:
            pass
        else:
            raise

class PlayerPosition:
    GK = 0
    DF = 1
    MF = 2
    FW = 3

    def __init__(self, string):
        string = string.strip()
        string = string[:2]
        if string == 'GK':
            self.pos = self.GK
        elif string == 'DF' or string == 'CB':
            self.pos = self.DF
        elif string == 'MF' or string == 'CM' or string == 'LM' or string == 'RM' or string == 'WI':
            self.pos = self.MF
        elif string == 'FW':
            self.pos = self.FW
        else:
            if string != '--':
                print >> sys.stderr, 'Unknown player position string: "%s"' % string
            self.pos = random.choice([self.GK] + 4 * [self.DF] + 4 * [self.MF] + 2 * [self.FW])

class Nationality:
    def __init__(self, string):
        self.nat = string
        if not self.nat:
            self.nat = 'N/A'

class PlayerConfigurator:
    def __init__(self, name, number, pos, nationality):
        self.name = name
        self.number = number
        self.pos = PlayerPosition(pos)
        self.nationality = Nationality(nationality)

    def createFreekick3Node(self, idnumber):
        playernode = etree.Element('Player')
        playernode.set('id', str(idnumber))
        namenode = etree.SubElement(playernode, 'Name')
        namenode.text = self.name
        skillsnode = etree.SubElement(playernode, 'Skills')
        # TODO: set skills correctly
        shotpower = 0.2
        passing = 0.2
        runspeed = 0.2
        ballcontrol = 0.2
        tackling = 0.2
        heading = 0.2
        goalkeeping = 0.2
        etree.SubElement(skillsnode, 'ShotPower').text = str(shotpower)
        etree.SubElement(skillsnode, 'Passing').text = str(passing)
        etree.SubElement(skillsnode, 'RunSpeed').text = str(runspeed)
        etree.SubElement(skillsnode, 'BallControl').text = str(ballcontrol)
        etree.SubElement(skillsnode, 'Tackling').text = str(tackling)
        etree.SubElement(skillsnode, 'Heading').text = str(heading)
        etree.SubElement(skillsnode, 'GoalKeeping').text = str(goalkeeping)
        return playernode

def createKitNode(node):
    outnode = etree.Element('Kit')
    outnode.set('type', node.get('type'))
    for child in node:
        if child.tag == 'Body':
            tname = 'ShirtColor1'
        elif child.tag == 'Body2':
            tname = 'ShirtColor2'
        elif child.tag == 'Shorts':
            tname = 'ShortsColor'
        elif child.tag == 'Socks':
            tname = 'SocksColor'
        else:
            continue
        r = int(child.get('r'))
        g = int(child.get('g'))
        b = int(child.get('b'))
        n = etree.SubElement(outnode, tname)
        n.set('r', str(r))
        n.set('g', str(g))
        n.set('b', str(b))
    return outnode

class Converter:
    def __init__(self):
        self.nextteamid = 1
        self.nextplayerid = 1
        self.playerdb = dict()

    def createLeagueNode(self, leaguefilename):
        out_playernodes = []
        out_leaguenode = etree.Element('League')
        in_leaguetree = etree.parse(leaguefilename)
        in_leagueroot = in_leaguetree.getroot()
        assert in_leagueroot.tag == 'League'
        title = in_leagueroot.get('title')
        out_leaguenode.set('name', title)
        for in_groupnode in in_leagueroot.xpath('/League/Group'):
            for in_teamnode in in_groupnode.xpath('Team'):
                out_teamnode = etree.SubElement(out_leaguenode, 'Team')
                out_teamnode.set('id', str(self.nextteamid))
                self.nextteamid += 1
                teamname = in_teamnode.get('name')
                etree.SubElement(out_teamnode, 'Name').text = teamname
                out_playernode = etree.SubElement(out_teamnode, 'Players')

                for in_playernode in in_teamnode.xpath('Player'):
                    # TODO: create dummy players and cut off too many players as appropriate
                    player_name = in_playernode.get('name')
                    player_num  = in_playernode.get('number')
                    player_pos  = in_playernode.get('pos')
                    player_nat  = in_playernode.get('nationality')
                    pc = PlayerConfigurator(player_name, player_num, player_pos, player_nat)
                    playernode = pc.createFreekick3Node(self.nextplayerid)
                    etree.SubElement(out_playernode, 'Player').set('id', str(self.nextplayerid))
                    self.nextplayerid += 1
                    out_playernodes.append(playernode)

                out_kitsnode = etree.SubElement(out_teamnode, 'Kits')
                for in_kitnode in in_teamnode.xpath('Kit'):
                    out_kitnode = createKitNode(in_kitnode)
                    out_kitsnode.append(out_kitnode)
            break # TODO: support more than one group
        return out_leaguenode, int(in_leagueroot.get('level_number')), out_playernodes

    def collect(self, indir, outdir):
        if not os.path.isdir(indir):
            print >> sys.stderr, 'Could not find input directory: "%s"' % indir
            sys.exit(1)
        if os.path.isdir(outdir):
            print >> sys.stderr, 'Output directory ("%s") already exists - aborting.' % outdir
            sys.exit(1)

        listing = glob.glob(os.path.join(indir, '*'))
        confederations = [l for l in listing if os.path.isdir(l)]
        teamsroot = etree.Element("Teams")
        playersroot = etree.Element("Players")
        for confederation in confederations:
            continentnode = etree.SubElement(teamsroot, 'Continent')
            assert confederation
            continentnode.set('name', os.path.basename(confederation))
            countries = glob.glob(os.path.join(confederation, '*'))
            countries = [l for l in countries if os.path.isdir(l)]
            for country in countries:
                leaguesystemnode = etree.SubElement(continentnode, 'LeagueSystem')
                assert country
                leaguesystemnode.set('name', os.path.basename(country).decode('utf-8'))
                leagues = glob.glob(os.path.join(country, '*.xml'))
                leaguenodes = dict()
                for league in leagues:
                    leaguenode, levelnum, playernodes = self.createLeagueNode(league)
                    if levelnum in leaguenodes:
                        print >> sys.stderr, "Warning: level number %d already in use, won't include %s" % (levelnum, leaguenode.get('name'))
                        continue
                    leaguenodes[levelnum] = leaguenode
                    for pn in playernodes:
                        playersroot.append(pn)
                for levelnum, league in sorted(leaguenodes.items()):
                    leaguesystemnode.append(league)

        mkdir_p(outdir)
        with open(os.path.join(outdir, 'Teams.xml'), 'w') as f:
            f.write(etree.tostring(teamsroot, pretty_print=True))
        with open(os.path.join(outdir, 'Players.xml'), 'w') as f:
            f.write(etree.tostring(playersroot, pretty_print=True))



def usage():
    print 'Usage: %s <input directory> <output directory>' % sys.argv[0]
    print '\tConverts football-data-fetcher output to Freekick3 data files.'

def main():
    if len(sys.argv) != 3:
        usage()
        sys.exit(1)

    indir = sys.argv[1]
    outdir = sys.argv[2]

    c = Converter()
    c.collect(indir, outdir)

if __name__ == '__main__':
    main()

