#!/usr/bin/env python2
# coding: utf-8

import sys
import os
import glob
from lxml import etree
import random
import re

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
    def __init__(self, name, number, pos, nationality, teamstrength):
        self.name = name
        self.number = number
        self.pos = PlayerPosition(pos)
        self.nationality = Nationality(nationality)
        self.teamstrength = teamstrength

    def createFreekick3Node(self, idnumber):
        playernode = etree.Element('Player')
        playernode.set('id', str(idnumber))
        namenode = etree.SubElement(playernode, 'Name')
        namenode.text = self.name
        skillsnode = etree.SubElement(playernode, 'Skills')
        # TODO: set skills correctly
        shotpower = self.teamstrength
        passing = self.teamstrength
        runspeed = self.teamstrength
        ballcontrol = self.teamstrength
        tackling = self.teamstrength
        heading = self.teamstrength
        goalkeeping = self.teamstrength
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

def shortenName(string):
    maxStringLen = 23
    stringLen = len(string)
    if stringLen <= maxStringLen:
        return string
    names = string.split()
    shortername = ''
    shorterStringLen = stringLen
    for n in names[:-1]:
        if len(n) > 2 and shorterStringLen > maxStringLen:
            shortername += n[0] + '. '
            shorterStringLen -= (len(n) - 2)
        else:
            shortername += n + ' '
    shortername += names[-1]
    if len(shortername) <= maxStringLen:
        return shortername
    return shortername[:(maxStringLen - 2)].strip() + '.'

class Converter:
    def __init__(self):
        self.nextteamid = 1
        self.nextplayerid = 1
        self.playerdb = dict()

    def createLeagueNode(self, in_leagueroot, bottomteam_strength, topteam_strength):
        out_playernodes = []
        out_leaguenode = etree.Element('League')
        title = in_leagueroot.get('title')
        out_leaguenode.set('name', title)
        for in_groupnode in in_leagueroot.xpath('/League/Group'):
            if len(out_leaguenode) > 0:
                print "Warning: already have a group, won't include group '%s'." % in_groupnode.get('title')
                continue
            in_teamnodes = in_groupnode.xpath('Team')
            num_teams = len(in_teamnodes)
            for in_teamnode in in_teamnodes:
                teamname = in_teamnode.get('name')
                teamname = re.sub(' F\.C\.$', '', teamname)
                teamname = re.sub('^F\.C\. ', '', teamname)
                teamname = shortenName(teamname.strip())
                postext = in_teamnode.get('position')
                if postext:
                    teampos = int(in_teamnode.get('position'))
                else:
                    teampos = 0
                if teampos <= 0:
                    teampos = num_teams / 2
                this_team_strength = topteam_strength - ((topteam_strength - bottomteam_strength) * (teampos / float(num_teams)))
                out_teamnode = etree.SubElement(out_leaguenode, 'Team')
                out_teamnode.set('id', str(self.nextteamid))
                self.nextteamid += 1
                etree.SubElement(out_teamnode, 'Name').text = teamname
                out_playernode = etree.SubElement(out_teamnode, 'Players')

                for in_playernode in in_teamnode.xpath('Player'):
                    # TODO: cut off too many players as appropriate
                    player_name = in_playernode.get('name')
                    player_num  = in_playernode.get('number')
                    player_pos  = in_playernode.get('pos')
                    player_nat  = in_playernode.get('nationality')
                    pc = PlayerConfigurator(shortenName(player_name), player_num, player_pos, player_nat, this_team_strength)
                    playernode = pc.createFreekick3Node(self.nextplayerid)
                    etree.SubElement(out_playernode, 'Player').set('id', str(self.nextplayerid))
                    self.nextplayerid += 1
                    out_playernodes.append(playernode)

                generatedPlayerNum = 1
                while len(out_playernode) < 16:
                    # Lower strength for dummy players
                    playernode = PlayerConfigurator('Player ' + str(generatedPlayerNum), 0, '--', 'N/A',
                        this_team_strength * 0.93).createFreekick3Node(self.nextplayerid)
                    etree.SubElement(out_playernode, 'Player').set('id', str(self.nextplayerid))
                    self.nextplayerid += 1
                    out_playernodes.append(playernode)
                    generatedPlayerNum += 1

                out_kitsnode = etree.SubElement(out_teamnode, 'Kits')
                for in_kitnode in in_teamnode.xpath('Kit'):
                    out_kitnode = createKitNode(in_kitnode)
                    out_kitsnode.append(out_kitnode)
        return out_leaguenode, out_playernodes

    def collect(self, orderingfile, indir, outdir):
        if not os.path.isdir(indir):
            print >> sys.stderr, 'Could not find input directory: "%s"' % indir
            sys.exit(1)
        if os.path.isdir(outdir):
            print >> sys.stderr, 'Output directory ("%s") already exists - aborting.' % outdir
            sys.exit(1)
        orderinglist = open(orderingfile, 'r').read().split('\n')
        ordering = dict()
        for num, country in enumerate(orderinglist):
            if country in ordering:
                print >> sys.stderr, 'Country "%s" found twice in the ordering file.' % country
                sys.exit(1)
            ordering[country] = 1.0 - (num / float(len(orderinglist)))

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
            leaguesystemnodes = dict()
            for country in countries:
                leaguesystemnode = etree.Element('LeagueSystem')
                assert country
                countryname = os.path.basename(country).decode('utf-8')
                if countryname not in ordering:
                    print >> sys.stderr, 'Country "%s" not found in the ordering file - aborting.' % countryname
                    sys.exit(1)

                leaguesystem_strength = ordering[countryname]
                leaguesystemnodes[leaguesystem_strength] = leaguesystemnode

                leaguesystemnode.set('name', countryname)
                leagues = glob.glob(os.path.join(country, '*.xml'))
                leaguenodes = dict()
                for league in leagues:
                    leagueroot = etree.parse(league).getroot()
                    assert leagueroot.tag == 'League'
                    levelnum = int(leagueroot.get('level_number'))
                    assert levelnum > 0, 'League "%s" has non-positive level number' % league
                    league_strength_max = leaguesystem_strength * (0.8 ** (levelnum - 1))
                    league_strength_min = leaguesystem_strength * (0.8 ** (levelnum))
                    leaguenode, playernodes = self.createLeagueNode(leagueroot, league_strength_min, league_strength_max)
                    if len(leaguenode) == 0:
                        continue
                    if levelnum in leaguenodes:
                        print >> sys.stderr, "Warning: level number %d already in use, won't include %s" % (levelnum, leaguenode.get('name'))
                        continue
                    leaguenodes[levelnum] = leaguenode
                    for pn in playernodes:
                        playersroot.append(pn)
                for levelnum, league in sorted(leaguenodes.items()):
                    leaguesystemnode.append(league)

            for leaguesystem_strength, leaguesystemnode in sorted(leaguesystemnodes.items(), reverse = True):
                continentnode.append(leaguesystemnode)

        mkdir_p(outdir)
        with open(os.path.join(outdir, 'Teams.xml'), 'w') as f:
            f.write(etree.tostring(teamsroot, pretty_print=True))
        with open(os.path.join(outdir, 'Players.xml'), 'w') as f:
            f.write(etree.tostring(playersroot, pretty_print=True))



def usage():
    print 'Usage: %s <ordering file> <input directory> <output directory>' % sys.argv[0]
    print '\tConverts football-data-fetcher output to Freekick3 data files.'
    print '\tOrdering file must consist of a list of countries, ordered from best to worst.'

def main():
    if len(sys.argv) != 4:
        usage()
        sys.exit(1)

    orderingfile = sys.argv[1]
    indir = sys.argv[2]
    outdir = sys.argv[3]

    c = Converter()
    c.collect(orderingfile, indir, outdir)

if __name__ == '__main__':
    main()

